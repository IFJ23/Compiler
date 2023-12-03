// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)

#include "parser.h"

Parser parser;

int parserInit() {
    Stack *s = malloc(sizeof(Stack));
    if (s == NULL)
        return INTERNAL_ERROR;
    stackInit(s);
    Stack *sUndef = malloc(sizeof(Stack));
    if (sUndef == NULL)
        return INTERNAL_ERROR;
    stackInit(sUndef);
    Symtable *st = symtableInit(1024);
    if (st == NULL)
        return INTERNAL_ERROR;
    Symtable *stLoc = symtableInit(1024);
    if (stLoc == NULL)
        return INTERNAL_ERROR;
    parser.stack = s;
    parser.undefStack = sUndef;
    parser.outsideBody = false;
    parser.symtable = st;
    parser.localSymtable = stLoc;
    parser.condDec = false;
    parser.currFunc = "main";

    genPrintHead();
    genCheckTruth();


    return 0;
}

void parserDestroy() {
    symtableFree(parser.symtable);
    symtableFree(parser.localSymtable);
    stackFree(parser.stack);
    stackFree(parser.undefStack);
    free(parser.stack);
}

int parseBody(Scanner *scanner);

int parsePeBody(Scanner *scanner) {
    int err = 0;

    if (parser.currToken.type == TYPE_RIGHT_CURLY_BRACKET)
        return 0;

    else {
        CHECKRULE(parseBody(scanner))
        GETTOKEN(scanner, &parser.currToken)

        CHECKRULE(parsePeBody(scanner))
    }

    return err;
}

int parseWhile(Scanner *scanner) {
    int err = 0;
    static int whileCnt = 0;
    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type != TYPE_LEFT_BRACKET) {
        printError(LINENUM, "While condition has to be wrapped by brackets.");
        return SYNTAX_ERROR;
    }

    whileCnt++;
    int currWhile = whileCnt;
    genWhileLoop1(currWhile);

    CHECKRULE(parseExpression(scanner, true))

    genWhileLoop2(currWhile);

    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET) {
        printError(LINENUM, "The body of while has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    parser.condDec = true;

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parsePeBody(scanner))

    parser.condDec = false;

    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET) {
        printError(LINENUM, "The body of while has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    genWhileLoop3(currWhile);

    return err;
}

int parseIf(Scanner *scanner) {
    int err = 0;
    static int ifCnt = 0;
    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_LET) {
        GETTOKEN(scanner, &parser.currToken)
        if (parser.currToken.type != TYPE_IDENTIFIER_VAR) {
            printError(LINENUM, "Let has to be followed by a variable.");
            return SYNTAX_ERROR;
        }

        if (symtableFind(parser.outsideBody ? parser.localSymtable : parser.symtable,
                         parser.currToken.value.string) == NULL) {
            printError(LINENUM, "Passing an undefined let to the if.");
            return SEMANTIC_UNDEFINED_ERROR;
        }

        GETTOKEN(scanner, &parser.currToken)

    } else {
        if (parser.currToken.type != TYPE_LEFT_BRACKET) {
            printError(LINENUM, "If condition has to be wrapped by brackets.");
            return SYNTAX_ERROR;
        }

        CHECKRULE(parseExpression(scanner, true))
    }

    Token tmp = {.type = DOLLAR};
    stackPush(parser.undefStack, tmp);

    ifCnt++;
    int currentCnt = ifCnt;
    genIfElse1(currentCnt);

    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET) {
        printError(LINENUM, "The body of if has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    parser.condDec = true;

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parsePeBody(scanner))

    parser.condDec = false;

    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET) {
        printError(LINENUM, "The body of if has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type != TYPE_KW || parser.currToken.value.kw != KW_ELSE) {
        printError(LINENUM, "If has to have else clause.");
        return SYNTAX_ERROR;
    }

    genIfElse2(currentCnt);

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET) {
        printError(LINENUM, "The body of else has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    Token middleDelimiter = {.type = REDUCED};
    stackPush(parser.undefStack, middleDelimiter);

    parser.condDec = true;

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parsePeBody(scanner))

    parser.condDec = false;

    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET) {
        printError(LINENUM, "The body of else has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    genIfElse3(currentCnt);

    return err;
}

int parseReturn(Scanner *scanner) {
    int err = 0;
    GETTOKEN(scanner, &parser.currToken)

    Keyword returning = KW_NIL;
    if (strcmp(parser.currFunc, "main") != 0) {
        SymtablePair *func = symtableFind(parser.symtable, parser.currFunc);
        if (func->data.parameters.itemCount != -1)
            returning = func->data.parameters.active->type;
    }

    if ((!parser.outsideBody || (parser.outsideBody && returning != KW_NIL))) {
        CHECKRULE(parseExpression(scanner, false))
        genReturn(parser.currFunc, true);
    } else {
        if ((parser.outsideBody) || (returning == KW_VOID)) {
            printError(LINENUM,
                       "Current function doesn't have a return value");
            return SYNTAX_ERROR;
        }
        genReturn(parser.currFunc, false);
    }

    return err;
}

int parseParamsCallN(Scanner *scanner, int *pc, SymtablePair *foundFunction) {
    int err = 0;
    if (foundFunction->data.paramsCnt == -1 || foundFunction->data.parameters.itemCount == -1) {
        switch (parser.currToken.type) {
            case TYPE_STRING:
            case TYPE_INT:
            case TYPE_DOUBLE:
                genStackPush(parser.currToken);
                ++(*pc);
                break;
            case TYPE_IDENTIFIER_VAR:
                if (symtableFind(parser.outsideBody ? parser.localSymtable : parser.symtable,
                                 parser.currToken.value.string) == NULL) {
                    printError(LINENUM, "Passing an undefined var to a function.");
                    return SEMANTIC_UNDEFINED_ERROR;
                }

                printf("PUSHS LF@%s\n", parser.currToken.value.string);
                ++(*pc);
                break;

            case TYPE_KW:
                if (parser.currToken.value.kw == KW_NIL) {
                    genStackPush(parser.currToken);
                    ++(*pc);
                    break;
                } else {
                    printError(LINENUM, "Function can only be called with a variable or a literal.");
                    return SYNTAX_ERROR;
                    break;
                }

            default:
                printError(LINENUM, "Function can only be called with a variable or a literal.");
                return SYNTAX_ERROR;
                break;
        }
    } else {
        LinkedList params = foundFunction->data.parameters;
        ListNode *prop = listGetByIndex(&params, *pc);
        if (prop && strcmp(prop->name, "_") != 0) {
            if (strcmp(prop->name, parser.currToken.value.string) != 0) {
                printError(LINENUM, "Name is wrong.");
                return SEMANTIC_OTHER_ERROR;
            } else {

                GETTOKEN(scanner, &parser.currToken)

                if (parser.currToken.type != TYPE_COLON) {
                    printError(LINENUM, "Expected ':' after variable name.");
                    return SYNTAX_ERROR;
                }

                GETTOKEN(scanner, &parser.currToken)
            }
        }
        switch (parser.currToken.type) {
            case TYPE_STRING:
            case TYPE_INT:
            case TYPE_DOUBLE:
                genStackPush(parser.currToken);
                ++(*pc);
                break;
            case TYPE_IDENTIFIER_VAR:
                if (symtableFind(parser.outsideBody ? parser.localSymtable : parser.symtable,
                                 parser.currToken.value.string) == NULL) {
                    printError(LINENUM, "Passing an undefined var to a function.");
                    return SEMANTIC_UNDEFINED_ERROR;
                }

                genStackPush(parser.currToken);
                ++(*pc);
                break;

            case TYPE_KW:
                if (parser.currToken.value.kw == KW_NIL) {
                    genStackPush(parser.currToken);
                    ++(*pc);
                    break;
                } else {
                    printError(LINENUM, "Function can only be called with a variable or a literal.");
                    return SYNTAX_ERROR;
                    break;
                }

            default:
                printError(LINENUM, "Function can only be called with a variable or a literal.");
                return SYNTAX_ERROR;
                break;
        }
    }

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type == TYPE_COMMA) {
        GETTOKEN(scanner, &parser.currToken)

        return parseParamsCallN(scanner, pc, foundFunction);
    } else
        return err;
}

int parseParamsCall(Scanner *scanner, int *pc, SymtablePair *foundFunction) {
    if (parser.currToken.type == TYPE_RIGHT_BRACKET)
        return 0;
    else
        return parseParamsCallN(scanner, pc, foundFunction);
}

int parseFunctionCall(Scanner *scanner) {
    int err = 0;

    SymtablePair *foundFunction = symtableFind(parser.symtable, parser.currToken.value.string);

    if (foundFunction == NULL) {
        printError(LINENUM, "Calling an undefined function.");
        return SEMANTIC_DEFINITION_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type != TYPE_LEFT_BRACKET) {
        printError(LINENUM, "Function's parameters have to be in a bracket (opening).");
        return SYNTAX_ERROR;
    }

    int parametersRealCount = 0;

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parseParamsCall(scanner, &parametersRealCount, foundFunction))

    if (!parser.outsideBody) {
        if ((parametersRealCount != foundFunction->data.paramsCnt) && foundFunction->data.paramsCnt != -1) {
            printError(LINENUM, "Wrong number of parameters passed");
            return SEMANTIC_TYPE_ERROR;
        }
    }

    if (parser.currToken.type != TYPE_RIGHT_BRACKET) {
        printError(LINENUM, "Function's parameters have to be in a bracket (closing).");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    ListNode *rv;
    if (foundFunction->data.parameters.itemCount == -1) {
        rv = NULL;
    } else {
        rv = foundFunction->data.parameters.active;
    }
    genFuncCall((char *) foundFunction->key, parametersRealCount, rv);

    return err;
}

int parseAssign(Scanner *scanner, Token variable) {
    int err = 0;
    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type == TYPE_IDENTIFIER_FUNC) {
        CHECKRULE(parseFunctionCall(scanner))
    } else {
        CHECKRULE(parseExpression(scanner, false))
    }

    if (parser.condDec) {
        stackPush(parser.undefStack, variable);
    }
    genAssignVariable(variable);

    return err;
}

int parseTypeP(LinkedList *ll, char *name) {
    if (parser.currToken.type != TYPE_KW) {
        printError(LINENUM, "Expected variable type. 1");
        return SYNTAX_ERROR;
    }

    switch (parser.currToken.value.kw) {
        case KW_STRING:
        case KW_INT:
        case KW_DOUBLE:
            if (ll != NULL) {
                listInsert(ll, parser.currToken.value.kw, name);
                ll->head->opt = false;
            }
            return 0;
        case KW_UNDEFINED_DOUBLE:
        case KW_UNDEFINED_INT:
        case KW_UNDEFINED_STRING:
            if (ll != NULL) {
                listInsert(ll, parser.currToken.value.kw, name);
                ll->head->opt = true;
            }
            return 0;

        default:
            printError(LINENUM, "Expected variable type.");
            return SYNTAX_ERROR;
    }
}

int parseVariableRedefinition(Scanner *scanner, Token variable) {
    int err = 0;

    SymtablePair *alrDefined = symtableFind(
            parser.outsideBody ? parser.localSymtable : parser.symtable, variable.value.string);

    if (alrDefined && alrDefined->data.type == LET) {
        printError(LINENUM, "It is not allowed to redefine constant.");
        return SEMANTIC_DEFINITION_ERROR;
    } else {
        parseAssign(scanner, variable);
    }

    return err;
}

int parseTypeN(Scanner *scanner, LinkedList *ll, char *name) {
    int err = 0;
    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parseTypeP(ll, name))
    ll->head->opt = true;
    return err;
}

int parseBody(Scanner *scanner) {
    int err = 0;

    if (parser.currToken.type == TYPE_KW)
        switch (parser.currToken.value.kw) {
            case KW_IF:
                CHECKRULE(parseIf(scanner))
                break;

            case KW_WHILE:
                CHECKRULE(parseWhile(scanner))
                break;

            case KW_RETURN:
                CHECKRULE(parseReturn(scanner))

                break;
            case KW_VAR:

                GETTOKEN(scanner, &parser.currToken)

                if (parser.currToken.type == TYPE_IDENTIFIER_VAR) {

                    Token variable = parser.currToken;

                    if (peek_token(scanner, &parser.currToken) != 0)
                        return LEXICAL_ERROR;

                    if (parser.currToken.type == TYPE_COLON) {

                        GETTOKEN(scanner, &parser.currToken)

                        GETTOKEN(scanner, &parser.currToken)

                        LinkedList ll;
                        listInit(&ll);

                        CHECKRULE(parseTypeP(&ll, variable.value.string))

                        if (peek_token(scanner, &parser.currToken) != 0)
                            return LEXICAL_ERROR;
                    }
                    if (parser.currToken.type == TYPE_ASSIGN) {

                        SymtablePair *alrDefined = symtableFind(
                                parser.outsideBody ? parser.localSymtable : parser.symtable, variable.value.string);
                        if (alrDefined == NULL) {
                            genDefineVariable(variable);
                        }
                        GETTOKEN(scanner, &parser.currToken)
                        CHECKRULE(parseAssign(scanner, variable))
                        LinkedList empty = {.itemCount = 0};
                        if (alrDefined == NULL || alrDefined->data.possiblyUndefined) {
                            symtableAdd(parser.outsideBody ? parser.localSymtable : parser.symtable,
                                        variable.value.string, VAR, -1, parser.condDec, empty);
                        }
                    } else {
                        CHECKRULE(parseExpression(scanner, false))
                    }
                }

                break;

            case KW_LET:

                GETTOKEN(scanner, &parser.currToken)

                if (parser.currToken.type == TYPE_IDENTIFIER_VAR) {

                    Token variable = parser.currToken;

                    if (peek_token(scanner, &parser.currToken) != 0)
                        return LEXICAL_ERROR;

                    if (parser.currToken.type == TYPE_COLON) {

                        GETTOKEN(scanner, &parser.currToken)

                        GETTOKEN(scanner, &parser.currToken)

                        LinkedList ll;
                        listInit(&ll);

                        CHECKRULE(parseTypeP(&ll, variable.value.string))

                        if (peek_token(scanner, &parser.currToken) != 0)
                            return LEXICAL_ERROR;

                    }
                    if (parser.currToken.type == TYPE_ASSIGN) {

                        SymtablePair *alrDefined = symtableFind(
                                parser.outsideBody ? parser.localSymtable : parser.symtable, variable.value.string);
                        if (alrDefined == NULL) {
                            genDefineVariable(variable);
                        }
                        GETTOKEN(scanner, &parser.currToken)
                        CHECKRULE(parseAssign(scanner, variable))
                        LinkedList empty = {.itemCount = 0};
                        if (alrDefined == NULL || alrDefined->data.possiblyUndefined) {
                            symtableAdd(parser.outsideBody ? parser.localSymtable : parser.symtable,
                                        variable.value.string, LET, -1, parser.condDec, empty);
                        }
                    } else {
                        CHECKRULE(parseExpression(scanner, false))
                    }
                }

                break;

            case KW_NIL:
                CHECKRULE(parseExpression(scanner, false))

                break;

            default:
                printError(LINENUM, "Unexpected keyword found.");
                return SYNTAX_ERROR;
                break;
        }
    else if (parser.currToken.type == TYPE_IDENTIFIER_FUNC) {
        CHECKRULE(parseFunctionCall(scanner))
    } else if (parser.currToken.type == TYPE_IDENTIFIER_VAR) {
        CHECKRULE(parseVariableRedefinition(scanner, parser.currToken))

    }
    else if (parser.currToken.type == TYPE_RETURN_ARROW) {
        return SYNTAX_ERROR;
    } else if (parser.currToken.type == TYPE_EOL) {
        return 0;
    } else {
        CHECKRULE(parseExpression(scanner, false))
    }

    return err;
}

int parseParamsDefN(Scanner *scanner, LinkedList *ll) {
    int err = 0;

    Token name = parser.currToken;
    Token id;

    if (parser.currToken.type != TYPE_IDENTIFIER_VAR) {
        printError(LINENUM, "Type has to be followed by a variable.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type != TYPE_IDENTIFIER_VAR) {
        printError(LINENUM, "Type has to be followed by a variable.");
        return SYNTAX_ERROR;
    }

    if (strcmp(parser.currToken.value.string, name.value.string) == 0) {
        printError(LINENUM, "Name and Id should be different.");
        return SEMANTIC_OTHER_ERROR;
    }

    if (parser.currToken.type == TYPE_IDENTIFIER_VAR) {
        id = parser.currToken;
        GETTOKEN(scanner, &parser.currToken)
    }

    if (parser.currToken.type != TYPE_COLON) {
        printError(LINENUM, "Expected ':' after variable name.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parseTypeP(ll, name.value.string))

//    ll->head->name = name.value.string;
//    ll->head->id = id.value.string;

    stackPush(parser.undefStack, id);

    LinkedList empty = {.itemCount = 0};
    symtableAdd(parser.localSymtable, id.value.string, VAR, -1, parser.condDec, empty);

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type == TYPE_COMMA) {
        GETTOKEN(scanner, &parser.currToken)

        return parseParamsDefN(scanner, ll);
    } else
        return err;
}

int parseParamsDef(Scanner *scanner, LinkedList *ll) {
    if (parser.currToken.type == TYPE_RIGHT_BRACKET)
        return 0;
    else
        return parseParamsDefN(scanner, ll);
}

int parseType(Scanner *scanner, LinkedList *ll) {
    if (parser.currToken.type == TYPE_OPTIONAL_TYPE) {
        return parseTypeN(scanner, ll, "return");
    } else if (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_VOID) {
        listInsert(ll, parser.currToken.value.kw, "return");
        return 0;
    }

    return parseTypeP(ll, "return");
}

int findDuplicateParams() {
    StackItem *curr = parser.undefStack->head;
    while (curr != NULL) {
        StackItem *next = curr->next;
        while (next != NULL) {
            if (strcmp(next->t.value.string, curr->t.value.string) == 0)
                return 1;
            next = next->next;
        }
        curr = curr->next;
    }
    return 0;
}

int parseFunctionDef(Scanner *scanner) {
    int err = 0;
    stackFree(parser.undefStack);
    parser.outsideBody = true;

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type != TYPE_IDENTIFIER_FUNC) {
        printError(LINENUM, "Keyword function has to be followed by function identifier.");
        return SYNTAX_ERROR;
    }
    parser.currFunc = parser.currToken.value.string;

    if (symtableFind(parser.symtable, parser.currToken.value.string) != NULL) {
        printError(LINENUM, "Redefinition of function.");
        return SEMANTIC_DEFINITION_ERROR;
    }

    Token func = parser.currToken;

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type != TYPE_LEFT_BRACKET) {
        printError(LINENUM, "Function definition has to have (empty) set of params.");
        return SYNTAX_ERROR;
    }

    LinkedList ll;
    listInit(&ll);
    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parseParamsDef(scanner, &ll))

    if (parser.currToken.type != TYPE_RIGHT_BRACKET) {
        printError(LINENUM, "Function definition has to have (empty) set of params.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type != TYPE_RETURN_ARROW) {
        printError(LINENUM, "Incorrect function definition syntax, missing colon.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parseType(scanner, &ll))

    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET) {
        printError(LINENUM, "The body of function has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    symtableAdd(parser.symtable, func.value.string, FUNC, ll.itemCount - 1, parser.condDec, ll);

    if (findDuplicateParams() != 0) {
        return SEMANTIC_TYPE_ERROR;
    }

    genFuncDef1(func.value.string, ll.itemCount - 1, ll);

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parsePeBody(scanner))

    genFuncDef2(func.value.string);
    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET) {
        printError(LINENUM, "The body of function has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    parser.outsideBody = false;
    parser.currFunc = "main";
    stackFree(parser.undefStack);
    return err;
}

int parseMainBody(Scanner *scanner) {
    int err = 0;
    if (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_FUNC) {
        CHECKRULE(parseFunctionDef(scanner))
    } else if (parser.currToken.type == TYPE_EOF)
        return 0;
    else
        CHECKRULE(parseBody(scanner))

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parseMainBody(scanner))
    return err;
}

int parseProgramEnd(Scanner *scanner) {
    if (parser.currToken.type == TYPE_MORE) {
        GETTOKEN(scanner, &parser.currToken)

        if (parser.currToken.type == TYPE_EOF)
            return 0;
        else {
            printError(LINENUM, "Closing tag can only be followed by EOL.");
            return SYNTAX_ERROR;
        }
    } else if (parser.currToken.type == TYPE_EOF)
        return 0;
    else
        return SYNTAX_ERROR;
}

int parseProgram(Scanner *scanner) {
    int err = 0;
    CHECKRULE(parseMainBody(scanner))
    CHECKRULE(parseProgramEnd(scanner))
    return err;
}

int parse(Scanner *scanner) {
    LinkedList empty = {.itemCount = -1};
    symtableAdd(parser.symtable, "readString", FUNC, 0, false, empty);
    symtableAdd(parser.symtable, "readInt", FUNC, 0, false, empty);
    symtableAdd(parser.symtable, "readDouble", FUNC, 0, false, empty);
    symtableAdd(parser.symtable, "write", FUNC, -1, false, empty);
    symtableAdd(parser.symtable, "Int2Double", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "Double2Int", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "length", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "substring", FUNC, 3, false, empty);
    symtableAdd(parser.symtable, "ord", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "chr", FUNC, 1, false, empty);

    GETTOKEN(scanner, &parser.currToken)

    return parseProgram(scanner);

    return 0;
}