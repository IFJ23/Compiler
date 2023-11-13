#include "ast.h"
#include "stderr_message.h"
#include <stdlib.h>

#define AST_ALLOC_CHECK(ptr) do { if ((ptr) == NULL) { ast_error = AST_ERROR_INTERNAL; return; } } while(0)
#define AST_ALLOC_CHECK_RN(ptr) do { if ((ptr) == NULL) { ast_error = AST_ERROR_INTERNAL; return NULL; } } while(0)

#if AST_DEBUG

#include <signal.h>

#define print_error(result, msg, ...) stderr_message("ast", ERROR, (result), (msg),##__VA_ARGS__); raise(SIGTRAP)
#else
#define print_error(result, msg, ...) stderr_message("ast", ERROR, (result), (msg),##__VA_ARGS__)
#endif

ASTError ast_error = AST_NO_ERROR;

ASTNode *ast_node(ASTNodeType nodeType) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    AST_ALLOC_CHECK_RN(node);

    node->actionType = nodeType;
    return node;
}

void clean_ast(ASTNode *node) {
    if (node == NULL) return;
    clean_ast(node->left);
    clean_ast(node->right);

    switch (node->actionType) {
        case AST_LIST:
            for (unsigned i = 0; i < node->dataCount; i++) {
                clean_ast(node->data[i].astPtr);
            }
            break;
        case AST_CONST_STRING:
            free((void *) node->data[0].stringConstantValue);
            break;
        case AST_ID:
            if (node->inheritedDataType != CF_BLACK_HOLE) {
                node->data[0].symbolTableItemPtr->reference_counter--;
            }
    }
    free(node);
}

bool is_ast_empty(ASTNode *ast) {
    return ast == NULL || (ast->left == NULL && ast->right == NULL && ast->dataCount == 0);
}

ASTNode *ast_node_data(ASTNodeType nodeType, unsigned dataCount) {
    ASTNode *node = calloc(1, sizeof(ASTNode) + dataCount * sizeof(ASTNodeData));
    AST_ALLOC_CHECK_RN(node);

    node->actionType = nodeType;
    node->dataCount = dataCount;
    return node;
}

ASTNode *ast_leaf_single_data(ASTNodeType nodeType, ASTNodeData data) {
    ASTNode *node = ast_node_data(nodeType, 1);
    AST_ALLOC_CHECK_RN(node);

    node->data[0] = data;
    ast_infer_leaf_type(node);

    return node;
}

ASTNode *ast_node_list(unsigned dataCount) {
    return ast_node_data(AST_LIST, dataCount);
}

ASTNode *ast_leaf_id(STSymbol *idSymbolPtr) {
    return ast_leaf_single_data(AST_ID, (ASTNodeData) {.symbolTableItemPtr = idSymbolPtr});
}

ASTNode *ast_leaf_black_hole() {
    ASTNode *node = ast_node_data(AST_ID, 1);
    AST_ALLOC_CHECK_RN(node);
    node->data[0] = (ASTNodeData) {.symbolTableItemPtr = NULL};
    node->inheritedDataType = CF_BLACK_HOLE;

    return node;
}

ASTNode *ast_node_func_call(STSymbol *funcIdSymbolPtr, ASTNode *paramListNode) {
    ASTNode *idNode = ast_leaf_id(funcIdSymbolPtr);
    ASTNode *callNode = ast_node(AST_FUNC_CALL);

    if (idNode == NULL || callNode == NULL) {
        return NULL;
    }

    callNode->left = idNode;
    callNode->right = paramListNode;
    ast_infer_node_type(callNode);

    return callNode;
}

ASTNode *ast_leaf_consti(int i) {
    return ast_leaf_single_data(AST_CONST_INT, (ASTNodeData) {.intConstantValue = i});
}

ASTNode *ast_leaf_constf(double f) {
    return ast_leaf_single_data(AST_CONST_FLOAT, (ASTNodeData) {.floatConstantValue = f});
}

ASTNode *ast_leaf_consts(const char *s) {
    char *sCopy = malloc(strlen(s) + 1);
    strcpy(sCopy, s);
    return ast_leaf_single_data(AST_CONST_STRING, (ASTNodeData) {.stringConstantValue = sCopy});
}

ASTNode *ast_leaf_constb(bool b) {
    return ast_leaf_single_data(AST_CONST_BOOL, (ASTNodeData) {.boolConstantValue = b});
}

ASTNode *ast_get_list_root(ASTNode *innerNode) {
    ASTNode *n = innerNode;

    while (n != NULL) {
        if (n->parent->actionType == AST_LIST) {
            for (unsigned i = 0; i < n->parent->dataCount; i++) {
                if (n->parent->data[i].astPtr == n) {
                    return n->parent;
                }
            }
        }

        n = n->parent;
    }

    return NULL;
}

void ast_add_to_list(ASTNode *astList, ASTNode *node, unsigned dataIndex) {
    if (astList == NULL) {
        ast_error = AST_ERROR_INVALID_ARGUMENT;
        return;
    }

    if (astList->actionType != AST_LIST) {
        ast_error = AST_ERROR_INVALID_TARGET;
        return;
    }

    if (astList->dataCount <= dataIndex) {
        ast_error = AST_ERROR_INVALID_ARGUMENT;
        return;
    }

    node->parent = astList;
    astList->data[dataIndex].astPtr = node;
}

unsigned ast_push_to_list(ASTNode *astList, ASTNode *node) {
    if (astList == NULL) {
        ast_error = AST_ERROR_INVALID_ARGUMENT;
        return (unsigned) -1;
    }

    if (astList->dataPointerIndex == astList->dataCount) {
        ast_error = AST_ERROR_LIST_FULL;
        return (unsigned) -1;
    }

    ast_add_to_list(astList, node, astList->dataPointerIndex);
    return astList->dataPointerIndex++;
}

static bool strictInference = false;

#define ast_uninferrable(node) (node)->inheritedDataType = CF_UNKNOWN_UNINFERRABLE; return false

void ast_set_strict_inference_state(bool state) {
    strictInference = state;
}

bool ast_infer_leaf_type(ASTNode *node) {
    // Only ID and CONSTs should be leaf nodes
    if (node->inheritedDataType == CF_UNKNOWN_UNINFERRABLE) {
        return false;
    }

    if (node->inheritedDataType != CF_UNKNOWN) {
        return true;
    }

    if (node->actionType == AST_ID) {
        STSymbol *symb = node->data[0].symbolTableItemPtr;

        if (symb == NULL) {
            node->inheritedDataType = CF_UNKNOWN;
            return true;
            /*
            ast_error = AST_ERROR_SYMBOL_NOT_ASSIGNED;
            ast_uninferrable(node);
             */
        }

        if (symb->type == ST_SYMBOL_VAR) {
            node->inheritedDataType = symb->data.var_data.type;
        } else {
            if (!symb->data.func_data.defined) {
                // Function not yet defined: set ID type to UNKNOWN
                node->inheritedDataType = strictInference ? CF_UNKNOWN_UNINFERRABLE : DATA_TYPE_UNDEFINED;
                // If strict inference is on, return false; otherwise, this is ok, return true
                return !strictInference;
            }

            STParam *retType = symb->data.func_data.ret_types;

            if (retType == NULL) {
                node->inheritedDataType = DATA_TYPE_NIL;
            } else if (retType->next == NULL) {
                node->inheritedDataType = retType->type;

            // If strict inference is on, we can't have any unknowns for functions
            if (strictInference && node->inheritedDataType == DATA_TYPE_UNDEFINED) {
                print_error(COMPILER_RESULT_ERROR_SEMANTIC_GENERAL,
                            "Couldn't infer type for function '%s'.", symb->identifier);
                ast_uninferrable(node);
            }
        }

        return true;
    }

    // In this case, the result must never be UNKNOWN (this node is either CONST or not a valid leaf node)
    node->inheritedDataType = ast_data_type_for_node_type(node->actionType);
    return node->inheritedDataType != DATA_TYPE_UNDEFINED;
}

bool check_unary_node_children(ASTNode *node) {
    if (node->left == NULL) {
        ast_error = AST_ERROR_UNARY_OP_CHILD_NOT_ASSIGNED;
        return false;
    }

    if (node->left->actionType == AST_FUNC_CALL || node->left->hasInnerFuncCalls) {
        node->hasInnerFuncCalls = true;
    }

    if (node->left->inheritedDataType == DATA_TYPE_UNDEFINED) {
        if (!ast_infer_node_type(node->left)) {
            node->inheritedDataType = CF_UNKNOWN_UNINFERRABLE;
            return false;
        }
    }

    node->inheritedDataType = node->left->inheritedDataType;
    return true;
}

bool check_binary_node_children(ASTNode *node) {
    if (node->left == NULL || node->right == NULL) {
        ast_error = AST_ERROR_BINARY_OP_CHILDREN_NOT_ASSIGNED;
        return false;
    }

    if (node->left->hasInnerFuncCalls || node->right->hasInnerFuncCalls
        || node->left->actionType == AST_FUNC_CALL || node->right->actionType == AST_FUNC_CALL) {
        node->hasInnerFuncCalls = true;
    }

    //if (node->left->inheritedDataType == CF_UNKNOWN) {
    if (!ast_infer_node_type(node->left)) {
        node->inheritedDataType = CF_UNKNOWN_UNINFERRABLE;
        return false;
    }
    //}

    //if (node->right->inheritedDataType == CF_UNKNOWN) {
    if (!ast_infer_node_type(node->right)) {
        node->inheritedDataType = CF_UNKNOWN_UNINFERRABLE;
        return false;
    }
    //}

    if (node->left->inheritedDataType == CF_BLACK_HOLE) {
        if (node->actionType == AST_ASSIGN || node->actionType == AST_DEFINE) {
            node->inheritedDataType = node->right->inheritedDataType;
            return true;
        } else {
            node->inheritedDataType = CF_UNKNOWN_UNINFERRABLE;
            return false;
        }
    }

    if (node->right->inheritedDataType == CF_BLACK_HOLE) {
        node->inheritedDataType = CF_UNKNOWN_UNINFERRABLE;
        return false;
    }

    // If we got to a FUNC_CALL for a func that hasn't been yet defined,
    // the children might've ended up being CF_UNKNOWN.
    // In that case, this is not an error. We can try to infer the type from the second child for now,
    // if both are unknown, this node will be unknown as well.
    if (node->left->inheritedDataType == DATA_TYPE_UNDEFINED) {
        if (node->right->inheritedDataType == DATA_TYPE_UNDEFINED) {
            node->inheritedDataType = DATA_TYPE_UNDEFINED;
            return true;
        } else {
            node->inheritedDataType = node->right->inheritedDataType;

            if (node->left->actionType == AST_ID) {
                node->left->inheritedDataType = node->inheritedDataType;

                if (node->left->data[0].symbolTableItemPtr == NULL) {
                    if (strictInference) {
                        ast_uninferrable(node);
                    } else {
                        return true;
                    }
                }

                if (node->left->data[0].symbolTableItemPtr->type == ST_SYMBOL_VAR) {
                    node->left->data[0].symbolTableItemPtr->data.var_data.type = node->right->inheritedDataType;
                }
            }

            return true;
        }
    } else if (node->right->inheritedDataType == DATA_TYPE_UNDEFINED) {
        node->inheritedDataType = node->left->inheritedDataType;

        if (node->right->actionType == AST_ID) {
            node->right->inheritedDataType = node->inheritedDataType;

            if (node->right->data[0].symbolTableItemPtr == NULL) {
                if (strictInference) {
                    ast_uninferrable(node);
                } else {
                    return true;
                }
            }

            if (node->right->data[0].symbolTableItemPtr->type == ST_SYMBOL_VAR) {
                node->right->data[0].symbolTableItemPtr->data.var_data.type = node->left->inheritedDataType;
            }
        }

        return true;
    }

    if (strictInference &&
        (node->left->inheritedDataType == DATA_TYPE_UNDEFINED || node->right->inheritedDataType == DATA_TYPE_UNDEFINED)) {
        ast_uninferrable(node);
    }

    if (node->left->inheritedDataType != node->right->inheritedDataType) {
        ast_error = AST_ERROR_BINARY_OP_TYPES_MISMATCH;
        node->inheritedDataType = CF_UNKNOWN_UNINFERRABLE;
        return false;
    }

    node->inheritedDataType = node->left->inheritedDataType;
    return true;
}

ASTDataType check_nodes_matching(ASTNode *left, ASTNode *right, ASTNodeType rootType) {
    if (left->inheritedDataType == CF_BLACK_HOLE) {
        if (!ast_infer_node_type(right)) {
            return CF_UNKNOWN_UNINFERRABLE;
        }

        return right->inheritedDataType;
    }

    ASTNode *tmpNode = ast_node(rootType);
    tmpNode->left = left;
    tmpNode->right = right;
    if (!check_binary_node_children(tmpNode)) {
        free(tmpNode);
        return CF_UNKNOWN_UNINFERRABLE;
    }

    ASTDataType type = tmpNode->inheritedDataType;
    free(tmpNode);
    return type;
}

#define ast_check_arithmetic(node) \
    if ((node)->inheritedDataType != DATA_TYPE_INT && (node)->inheritedDataType != DATA_TYPE_DOUBLE && (node)->inheritedDataType != DATA_TYPE_UNDEFINED) { \
        stderr_message("ast", ERROR, COMPILER_RESULT_ERROR_TYPE_INCOMPATIBILITY_IN_EXPRESSION,                                     \
            "Unexpected or incompatible operands for arithmetic operator.\n");                                                     \
        ast_error = AST_ERROR_INVALID_CHILDREN_TYPE_FOR_OP;                                                                        \
        ast_uninferrable(node); \
}

#define ast_check_arithmetic_or_str(node) \
    if ((node)->inheritedDataType != DATA_TYPE_INT && (node)->inheritedDataType != DATA_TYPE_DOUBLE                \
            && (node)->inheritedDataType != DATA_TYPE_STRING && (node)->inheritedDataType != DATA_TYPE_UNDEFINED) { \
        stderr_message("ast", ERROR, COMPILER_RESULT_ERROR_TYPE_INCOMPATIBILITY_IN_EXPRESSION,      \
            "Unexpected or incompatible operands for arithmetic operator.\n");                          \
        ast_error = AST_ERROR_INVALID_CHILDREN_TYPE_FOR_OP;                                         \
        ast_uninferrable(node); \
}


bool assignment_inference_list_func_call(ASTNode *node) {
    ASTNode *leftIdListNode = node->left;
    ASTNode *rightFuncCallNode = node->right->data[0].astPtr;

    ASTNode *funcCallIdNode = rightFuncCallNode->left;

    if (!ast_infer_node_type(rightFuncCallNode)) {
        ast_uninferrable(node);
    }

    STSymbol *funcSymb = funcCallIdNode->data[0].symbolTableItemPtr;
    if (!funcSymb->data.func_data.defined) {
        // Let it be… the node's type will be unknown, if this is an assignment,
        // we could infer the lhs IDs' types from their previous usage – or not
        node->inheritedDataType = DATA_TYPE_UNDEFINED;
        return ast_infer_node_type(leftIdListNode);
    }

    if (leftIdListNode->dataCount != funcSymb->data.func_data.ret_types_count) {
        print_error(COMPILER_RESULT_ERROR_WRONG_PARAMETER_OR_RETURN_VALUE,
                    "Assignment left-hand side variables don't match return values of the right-hand side function '%s'.\n",
                    funcSymb->identifier);
        ast_uninferrable(node);
    }

    STParam *funcRetType = funcSymb->data.func_data.ret_types;
    for (unsigned i = 0; i < leftIdListNode->dataCount; i++) {
        ASTNode *leftIdNode = leftIdListNode->data[i].astPtr;

        bool strictInferenceState = strictInference;
        strictInference = false;

        if (!ast_infer_node_type(leftIdNode)) {
            print_error(COMPILER_RESULT_ERROR_SEMANTIC_GENERAL,
                        "Error deducing type for variable '%s'.\n",
                        leftIdNode->data[0].symbolTableItemPtr->identifier);
            ast_uninferrable(node);
        }
        strictInference = strictInferenceState;

        if (leftIdNode->inheritedDataType == CF_BLACK_HOLE) {
            funcRetType = funcRetType->next;
            continue;
        }

        if (leftIdNode->inheritedDataType != DATA_TYPE_UNDEFINED && funcRetType->type != DATA_TYPE_UNDEFINED) {
            if (leftIdNode->inheritedDataType != funcRetType->type) {
                print_error(COMPILER_RESULT_ERROR_SEMANTIC_GENERAL,
                            "Type of left-hand side variable '%s' doesn't match its corresponding right-hand side.\n",
                            leftIdNode->data[0].symbolTableItemPtr->identifier);
                ast_uninferrable(node);
            }
        } else if (leftIdNode->inheritedDataType == DATA_TYPE_UNDEFINED && funcRetType->type != DATA_TYPE_UNDEFINED) {
            leftIdNode->inheritedDataType = funcRetType->type;
            leftIdNode->data[0].symbolTableItemPtr->data.var_data.type = funcRetType->type;
        } else if (leftIdNode->inheritedDataType != DATA_TYPE_UNDEFINED && funcRetType->type == DATA_TYPE_UNDEFINED) {
            funcRetType->type = leftIdNode->inheritedDataType;
        } // if both are unknown, move on

        funcRetType = funcRetType->next;
    }

    node->inheritedDataType = DATA_TYPE_NIL;
    return true;
}

bool assignment_inference_semantic_checks(ASTNode *node) {
    if (node->left->actionType == AST_LIST) {
        // TODO: break into multiple ifs to provide (better) error messages

        if (node->right->actionType == AST_LIST && node->right->dataCount == 1
            && node->right->data[0].astPtr->actionType == AST_FUNC_CALL) {
            return assignment_inference_list_func_call(node);
        }

        if (node->right->actionType != AST_LIST || node->right->dataCount != node->left->dataCount) {
            print_error(COMPILER_RESULT_ERROR_SEMANTIC_GENERAL,
                        "Number of variables and assigned values don't match.\n");
            ast_uninferrable(node);
        }

        for (unsigned i = 0; i < node->left->dataCount; i++) {
            ASTNode *leftIdNode = node->left->data[i].astPtr;
            ASTNode *rightValueNode = node->right->data[i].astPtr;

            bool strictInferenceState = strictInference;
            strictInference = false;
            if (!ast_infer_node_type(leftIdNode)) {
                print_error(COMPILER_RESULT_ERROR_TYPE_INCOMPATIBILITY_IN_EXPRESSION,
                            "Error deducing type for variable '%s'.\n",
                            leftIdNode->data[0].symbolTableItemPtr->identifier);
                ast_uninferrable(node);
            }
            strictInference = strictInferenceState;

            if (!ast_infer_node_type(rightValueNode)) {
                print_error(COMPILER_RESULT_ERROR_TYPE_INCOMPATIBILITY_IN_EXPRESSION,
                            "Error deducing type for variable '%s'.\n",
                            leftIdNode->data[0].symbolTableItemPtr->identifier);
                ast_uninferrable(node);
            }

            if (check_nodes_matching(leftIdNode, rightValueNode, node->actionType) == CF_UNKNOWN_UNINFERRABLE) {
                print_error(COMPILER_RESULT_ERROR_TYPE_INCOMPATIBILITY_IN_EXPRESSION,
                            "Type of left-hand side variable '%s' doesn't match its corresponding right-hand side.\n",
                            leftIdNode->data[0].symbolTableItemPtr->identifier);
                ast_uninferrable(node);
            }
        }
    } else {
        if (node->left->actionType != AST_ID) {
            print_error(COMPILER_RESULT_ERROR_SEMANTIC_GENERAL,
                        "Expected identifier on the left-hand side.\n");
            ast_uninferrable(node);
        }

        if (!ast_infer_node_type(node->left)) {
            print_error(COMPILER_RESULT_ERROR_TYPE_INCOMPATIBILITY_IN_EXPRESSION,
                        "Error deducing type for variable '%s'.\n", node->left->data[0].symbolTableItemPtr->identifier);
            ast_uninferrable(node);
        }

        if (!check_binary_node_children(node)) {
            print_error(COMPILER_RESULT_ERROR_TYPE_INCOMPATIBILITY_IN_EXPRESSION,
                        "Type of left-hand side variable '%s' doesn't match its corresponding right-hand side.\n",
                        node->left->data[0].symbolTableItemPtr->identifier);
            ast_uninferrable(node);
        }
    }

    node->inheritedDataType = DATA_TYPE_NIL;
    return true;
}

bool func_call_inference_semantic_checks(ASTNode *node) {
    ASTNode *leftFuncIdNode = node->left;
    ASTNode *rightFuncParamsNode = node->right;

    STSymbol *funcSymbol = leftFuncIdNode->data[0].symbolTableItemPtr;
    STFunctionData *funcData = &funcSymbol->data.func_data;

    if (funcSymbol == NULL || funcSymbol->type != ST_SYMBOL_FUNC) {
        print_error(COMPILER_RESULT_ERROR_INTERNAL, "Invalid symbol assigned to function call node.\n");
        ast_uninferrable(node);
    }

    if (strcmp(funcSymbol->identifier, "print") == 0) {
        // print is kinda unique with its variable argument count
        // let's just infer the arguments and call it a day

        if (rightFuncParamsNode != NULL) {
            if (!ast_infer_node_type(rightFuncParamsNode)) {
                ast_uninferrable(node);
            }
        }

        node->inheritedDataType = DATA_TYPE_NIL;
        return true;
    }

    if (!funcData->defined) {
        // Function is not defined, that's ok, if strict inference is off.
        if (strictInference) {
            leftFuncIdNode->inheritedDataType = CF_UNKNOWN_UNINFERRABLE;
            print_error(COMPILER_RESULT_ERROR_UNDEFINED_OR_REDEFINED_FUNCTION_OR_VARIABLE,
                        "Function '%s' isn't defined.\n", funcSymbol->identifier);

            ast_uninferrable(node);
        }

        // Run inference for its parameters.
        node->inheritedDataType = DATA_TYPE_UNDEFINED;
        leftFuncIdNode->inheritedDataType = DATA_TYPE_UNDEFINED;

        if (rightFuncParamsNode != NULL) {
            return ast_infer_node_type(rightFuncParamsNode);
        }

        return true;
    }

    if (!ast_infer_node_type(leftFuncIdNode)) {
        print_error(COMPILER_RESULT_ERROR_INTERNAL, "Couldn't infer function '%s' return value.\n",
                    funcSymbol->identifier);
        ast_uninferrable(node);
    }

    node->inheritedDataType = leftFuncIdNode->inheritedDataType;

    if (rightFuncParamsNode != NULL && rightFuncParamsNode->actionType == AST_LIST) {
        STParam *par = funcData->params;
        for (unsigned i = 0; i < rightFuncParamsNode->dataCount; i++) {
            if (par == NULL) {
                print_error(COMPILER_RESULT_ERROR_WRONG_PARAMETER_OR_RETURN_VALUE,
                            "Too many arguments in function '%s' call.\n", funcSymbol->identifier);
                ast_uninferrable(node);
            }

            ASTNode *parAst = rightFuncParamsNode->data[i].astPtr;
            if (!ast_infer_node_type(parAst)) {
                print_error(COMPILER_RESULT_ERROR_TYPE_INCOMPATIBILITY_IN_EXPRESSION,
                            "Error deducing type for the argument number %u of function '%s'.\n", i,
                            funcSymbol->identifier);
                ast_uninferrable(node);
            }

            if (parAst->hasInnerFuncCalls || parAst->actionType == AST_FUNC_CALL) {
                node->hasInnerFuncCalls = true;
            }

            if (par->type == DATA_TYPE_UNDEFINED) {
                par->type = parAst->inheritedDataType;
                return true;
            } else {
                if (par->type != parAst->inheritedDataType && parAst->inheritedDataType != DATA_TYPE_UNDEFINED) {
                    print_error(COMPILER_RESULT_ERROR_WRONG_PARAMETER_OR_RETURN_VALUE,
                                "Invalid type of the argument number %u in function '%s' call.\n", i,
                                funcSymbol->identifier);
                    ast_uninferrable(node);
                }
            }

            par = par->next;
        }

        if (par != NULL) {
            print_error(COMPILER_RESULT_ERROR_WRONG_PARAMETER_OR_RETURN_VALUE,
                        "Not enough arguments in function '%s' call.\n", funcSymbol->identifier);
            ast_uninferrable(node);
        }

        return true;
    } else {
        STParam *par = funcData->params;

        if (rightFuncParamsNode == NULL && par == NULL) {
            // Call has no arguments, function expects none
            return true;
        }

        if (rightFuncParamsNode != NULL && par == NULL) {
            // Call has arguments, function does not
            print_error(COMPILER_RESULT_ERROR_WRONG_PARAMETER_OR_RETURN_VALUE,
                        "Expected no arguments in function '%s' call.\n", funcSymbol->identifier);
            ast_uninferrable(node);
        }

        if (rightFuncParamsNode == NULL && par != NULL) {
            // Call has no arguments, function has some
            print_error(COMPILER_RESULT_ERROR_WRONG_PARAMETER_OR_RETURN_VALUE,
                        "Expected an argument in function '%s' call.\n", funcSymbol->identifier);
            ast_uninferrable(node);
        }

        if (par != NULL && par->next != NULL) {
            // Call has one argument, function has more
            print_error(COMPILER_RESULT_ERROR_WRONG_PARAMETER_OR_RETURN_VALUE,
                        "Unexpected number of arguments in function '%s' call.\n", funcSymbol->identifier);
            ast_uninferrable(node);
        }

        if (!ast_infer_node_type(rightFuncParamsNode)) {
            print_error(COMPILER_RESULT_ERROR_TYPE_INCOMPATIBILITY_IN_EXPRESSION,
                        "Error deducing type for an argument of function '%s'.\n", funcSymbol->identifier);
            ast_uninferrable(node);
        }

        if (par->type == DATA_TYPE_UNDEFINED) {
            par->type = rightFuncParamsNode->inheritedDataType;
            return true;
        } else {
            if (par->type != rightFuncParamsNode->inheritedDataType) {
                print_error(COMPILER_RESULT_ERROR_WRONG_PARAMETER_OR_RETURN_VALUE,
                            "Invalid argument type in function '%s' call.\n", funcSymbol->identifier);
                ast_uninferrable(node);
            }
        }
    }

    return true;
}

bool ast_infer_node_type(ASTNode *node) {
    if (node == NULL) return false;

    if (node->inheritedDataType == CF_UNKNOWN_UNINFERRABLE) {
        return false;
    }

    // In some cases, the outer node will have a correct type (based on siblings, for example)
    // while somewhere in the tree, there can still be an UNKNOWN function call.
    // Without this, a lot of inference steps will be called redundantly, but at this point,
    // it's better than having type errors. This issue should be looked into nevertheless.
    /*if (node->inheritedDataType != CF_UNKNOWN && node->inheritedDataType != CF_NIL) {
        return true;
    }*/

    switch (node->actionType) {
        case AST_ADD:
            if (!check_binary_node_children(node)) return false;
            // Strings can be added together using + as well.
            ast_check_arithmetic_or_str(node);
            return true;
        case AST_SUBTRACT:
        case AST_MULTIPLY:
        case AST_DIVIDE:
            if (!check_binary_node_children(node)) return false;
            ast_check_arithmetic(node);
            return true;

        case AST_AR_NEGATE:
            if (!check_unary_node_children(node)) return false;
            ast_check_arithmetic(node);
            return true;

        case AST_LOG_LT:
        case AST_LOG_GT:
        case AST_LOG_LTE:
        case AST_LOG_GTE:
            if (!check_binary_node_children(node)) return false;
            ast_check_arithmetic_or_str(node); // Comparisons may only be done on ints, floats or strings
//            node->inheritedDataType = CF_BOOL; // but the result is BOOL
            return true;

        case AST_LOG_NOT:
            if (!check_unary_node_children(node)) return false;
            ast_check_logic(node);
            return true;

        case AST_LOG_AND:
        case AST_LOG_OR:
            if (!check_binary_node_children(node)) return false;
            ast_check_logic(node);
            return true;

        case AST_LOG_EQ:
        case AST_LOG_NEQ:
            if (!check_binary_node_children(node)) return false;
//            node->inheritedDataType = CF_BOOL;
            return true;

        case AST_DEFINE:
            if (node->left->actionType != AST_LIST && node->left->inheritedDataType == CF_BLACK_HOLE) {
                print_error(COMPILER_RESULT_ERROR_SEMANTIC_GENERAL,
                            "Expected a name of variable on the left-hand side of a definition statement.\n");
                ast_uninferrable(node);
            } else if (node->left->actionType == AST_LIST) {
                for (unsigned i = 0; i < node->left->dataCount; i++) {
                    if (node->left->data[i].astPtr->inheritedDataType != CF_BLACK_HOLE) {
                        goto case_ast_assign;
                    }
                }
                print_error(COMPILER_RESULT_ERROR_SEMANTIC_GENERAL,
                            "Expected at least one new variable on the left-hand side of a definition statement.\n");
                ast_uninferrable(node);
            }
        case_ast_assign:
        case AST_ASSIGN:
            return assignment_inference_semantic_checks(node);

        case AST_FUNC_CALL:
            // AST_FUNC_CALL node is represented as a binary node with an AST_ID node as the left child
            // and AST_LIST as the right child. Its type can thus be inferred from the corresponding AST_ID
            // node, which will be inferred from the symbol table using ast_infer_leaf_type().
            // The right child is not important in this case, so calling check_unary_node_children() is sufficient.
            // We'll run inference for the right subtree nevertheless.
            return func_call_inference_semantic_checks(node);

        case AST_LIST:
            if (node->dataCount == 0) {
                node->inheritedDataType = DATA_TYPE_NIL;
            } else if (node->dataCount == 1) {
                ASTNode *innerNode = node->data[0].astPtr;

                if (innerNode == NULL
                    || (innerNode->inheritedDataType == CF_UNKNOWN_UNINFERRABLE)
                    || (!ast_infer_node_type(innerNode))) {
                    ast_uninferrable(node);
                } else {
                    node->hasInnerFuncCalls = innerNode->hasInnerFuncCalls;
                    node->inheritedDataType = innerNode->inheritedDataType;
                }
            } else {
//                node->inheritedDataType = CF_MULTIPLE;
                // Run inference for the inner trees
                bool hasError = false;
                bool hasFuncCall = false;
                for (unsigned i = 0; i < node->dataCount; i++) {
                    hasError |= ast_infer_node_type(node->data[i].astPtr);
                    if (node->data[i].astPtr != NULL) {
                        hasFuncCall |= node->data[i].astPtr->hasInnerFuncCalls;
                    }
                }

                node->hasInnerFuncCalls = hasFuncCall;
                return hasError;
            }

            return true;

        case AST_ID:
        case AST_CONST_INT:
        case AST_CONST_FLOAT:
        case AST_CONST_STRING:
        case AST_CONST_BOOL:
            return ast_infer_leaf_type(node);
    }

    return false;
}

ASTDataType ast_data_type_for_node_type(ASTNodeType nodeType) {
    switch (nodeType) {
        case AST_LOG_NOT:
        case AST_LOG_AND:
        case AST_LOG_OR:
        case AST_LOG_EQ:
        case AST_LOG_NEQ:
        case AST_LOG_LT:
        case AST_LOG_GT:
        case AST_LOG_LTE:
        case AST_LOG_GTE:
        case AST_CONST_INT:
            return DATA_TYPE_INT;
        case AST_CONST_DOUBLE:
            return DATA_TYPE_DOUBLE;
        case AST_CONST_STRING:
            return DATA_TYPE_STRING;
        default:
            return DATA_TYPE_UNDEFINED;
    }
}