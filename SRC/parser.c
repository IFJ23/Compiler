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
    Symtable *stIfLoc = symtableInit(1024);
    if (stIfLoc == NULL)
        return INTERNAL_ERROR;
    parser.stack = s;
    parser.undefStack = sUndef;
    parser.outsideBody = false;
    parser.symtable = st;
    parser.localSymtable = stLoc;
    parser.ifSymtable = stIfLoc;
    parser.condDec = false;
    parser.ifScope = false;
    parser.currFunc = "main";

    genPrintHead();
    genCheckTruth();

    return 0;
}

void parserDestroy() {
    symtableFree(parser.symtable);
    symtableFree(parser.localSymtable);
    symtableFree(parser.ifSymtable);
    stackFree(parser.stack);
    stackFree(parser.undefStack);
    free(parser.stack);
}

void clearSymtable(Symtable *symtable) {
    symtableFree(symtable);
    symtable = symtableInit(1024); // Reinitialize the symtable
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


void addVariable(const char *varName, ElemType varType) {
    Symtable *targetSymtable = NULL;

    if (parser.ifScope) {
        targetSymtable = parser.ifSymtable;
    } else if (parser.outsideBody) {
        targetSymtable = parser.localSymtable;
    } else {
        targetSymtable = parser.symtable;
    }

    LinkedList empty = {.itemCount = 0};
    symtableAdd(targetSymtable, varName, varType, -1, parser.condDec, empty);
}

SymtablePair *findVariable(const char *varName) {
    SymtablePair *foundVar = NULL;

    if (parser.outsideBody && parser.ifScope) {
        foundVar = symtableFind(parser.ifSymtable, varName);
        if (foundVar == NULL) {
            foundVar = symtableFind(parser.localSymtable, varName);
            if (foundVar == NULL) {
                foundVar = symtableFind(parser.symtable, varName);
            }
        }
    } else if (parser.outsideBody) {
        foundVar = symtableFind(parser.localSymtable, varName);
        if (foundVar == NULL) {
            foundVar = symtableFind(parser.symtable, varName);
        }
    } else if (parser.ifScope) {
        foundVar = symtableFind(parser.ifSymtable, varName);
        if (foundVar == NULL) {
            foundVar = symtableFind(parser.symtable, varName);
        }
    } else {
        foundVar = symtableFind(parser.symtable, varName);
    }

    return foundVar;
}

int parseWhile(Scanner *scanner) {
    int err = 0;
    static int whileCnt = 0;
    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type == TYPE_LEFT_BRACKET)
    {
        CHECKRULE(parseExpression(scanner, true))
    } else {
        CHECKRULE(parseExpression(scanner, false))
    }

    whileCnt++;
    int currWhile = whileCnt;
    genWhileLoop1(currWhile);
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


        if (findVariable(parser.currToken.value.string) == NULL) {
            printError(LINENUM, "Passing an undefined let to the if.");
            return SEMANTIC_UNDEFINED_ERROR;
        }

        GETTOKEN(scanner, &parser.currToken)
    } else {
        if (parser.currToken.type == TYPE_LEFT_BRACKET) {
            CHECKRULE(parseExpression(scanner, true));
        } else {
            CHECKRULE(parseExpression(scanner, false));
        }
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
    parser.ifScope = true;

    GETTOKEN(scanner, &parser.currToken)
    stackFree(parser.undefStack);

    CHECKRULE(parsePeBody(scanner))

    stackFree(parser.undefStack);

    clearSymtable(parser.ifSymtable);
    parser.ifScope = false;
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
    parser.ifScope = true;
    stackFree(parser.undefStack);

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parsePeBody(scanner))

    clearSymtable(parser.ifSymtable);
    stackFree(parser.undefStack);
    parser.ifScope = false;
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
        if (func->value.parameters.itemCount != -1)
            returning = func->value.parameters.active->type;
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
    if (foundFunction->value.paramsCnt == -1 || foundFunction->value.parameters.itemCount == -1) {
        switch (parser.currToken.type) {
            case TYPE_STRING:
            case TYPE_INT:
            case TYPE_DOUBLE:
                genStackPush(parser.currToken);
                ++(*pc);
                break;
            case TYPE_IDENTIFIER_VAR:
                if (findVariable(parser.currToken.value.string) == NULL) {
                    if (parser.ifScope) {
                        printf("We are on the right path. \n");
                    }
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
        LinkedList params = foundFunction->value.parameters;
        ListNode *prop = listGetByIndex(&params, *pc);
        if (prop && strcmp(prop->name, "_") != 0) {
            if (strcmp(prop->name, parser.currToken.value.string) != 0) {
                printError(LINENUM, "Name is wrong.");
                return SEMANTIC_TYPE_ERROR;
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
                if (findVariable(parser.currToken.value.string) == NULL) {
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
        if ((parametersRealCount != foundFunction->value.paramsCnt) && foundFunction->value.paramsCnt != -1) {
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
    if (foundFunction->value.parameters.itemCount == -1) {
        rv = NULL;
    } else {
        rv = foundFunction->value.parameters.active;
    }
    genFuncCall((char *) foundFunction->key, parametersRealCount, rv);

    return err;
}

int parseAssign(Scanner *scanner, Token variable) {
    int err = 0;
    GETTOKEN(scanner, &parser.currToken)

    if (parser.currToken.type == TYPE_IDENTIFIER_FUNC) {
        CHECKRULE(parseFunctionCall(scanner))
    } else if (parser.currToken.type == TYPE_LEFT_BRACKET ||
               parser.currToken.type == TYPE_IDENTIFIER_VAR ||
               parser.currToken.type == TYPE_INT ||
               parser.currToken.type == TYPE_DOUBLE ||
               parser.currToken.type == TYPE_MULTILINE_STRING ||
               parser.currToken.type == TYPE_STRING) {
        CHECKRULE(parseExpression(scanner, false))
    } else {
        return SYNTAX_ERROR;
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

    SymtablePair *alrDefined = findVariable(variable.value.string);


    if (alrDefined && alrDefined->value.type == LET) {
        printError(LINENUM, "It is not allowed to redefine constant.");
        return SEMANTIC_DEFINITION_ERROR;
    } else if (alrDefined && alrDefined->value.type == VAR) {

        Token variable = parser.currToken;

        if (peek_token(scanner, &parser.currToken) != 0)
            return LEXICAL_ERROR;

        if (parser.currToken.type == TYPE_ASSIGN) {

            if (alrDefined == NULL) {
                genDefineVariable(variable);
            }
            GETTOKEN(scanner, &parser.currToken)
            CHECKRULE(parseAssign(scanner, variable))
            if (alrDefined == NULL || alrDefined->value.possiblyUndefined) {
                addVariable(variable.value.string, VAR);

            }
        } else {
            printError(LINENUM, "Syntax Error");
            return SYNTAX_ERROR;
        }
    } else {
        printError(LINENUM, "Undefined variable used in an expression.");
        return SEMANTIC_UNDEFINED_ERROR;
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

                if (symtableFind(parser.symtable, parser.currToken.value.string) != NULL) {
                    printError(LINENUM, "Redefinition of variable.");
                    return SEMANTIC_DEFINITION_ERROR;
                }

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

                        if (!ll.head->opt) {
                            if (parser.currToken.type != TYPE_ASSIGN)
                                return SEMANTIC_UNDECLARED_ERROR;
                        }
                    }

                    SymtablePair *alrDefined = symtableFind(parser.symtable, variable.value.string);
                    if (alrDefined == NULL) {
                        genDefineVariable(variable);
                    }

                    if (alrDefined == NULL || alrDefined->value.possiblyUndefined) {
                        addVariable(variable.value.string, VAR);
                    }

                    if (parser.currToken.type == TYPE_ASSIGN) {
                        GETTOKEN(scanner, &parser.currToken)
                        CHECKRULE(parseAssign(scanner, variable))
                    } else if (parser.currToken.type == TYPE_EOL) {
                        return 0;
                    } else {
                        printError(LINENUM, "Unexpected token found.");
                        return SYNTAX_ERROR;
                    }
                }

                break;

            case KW_LET:

                GETTOKEN(scanner, &parser.currToken)

                if (symtableFind(parser.symtable, parser.currToken.value.string) != NULL) {
                    printError(LINENUM, "Redefinition of variable.");
                    return SEMANTIC_DEFINITION_ERROR;
                }

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

                        if (!ll.head->opt) {
                            if (parser.currToken.type != TYPE_ASSIGN)
                                return SEMANTIC_UNDECLARED_ERROR;
                        }
                    }
                    if (parser.currToken.type == TYPE_ASSIGN) {

                        SymtablePair *alrDefined = symtableFind(parser.symtable, variable.value.string);
                        if (alrDefined == NULL) {
                            genDefineVariable(variable);
                        }
                        GETTOKEN(scanner, &parser.currToken)
                        CHECKRULE(parseAssign(scanner, variable))
                        if (alrDefined == NULL || alrDefined->value.possiblyUndefined) {
                            addVariable(variable.value.string, LET);
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
    } else if (parser.currToken.type == TYPE_RETURN_ARROW) {
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
        return SEMANTIC_TYPE_ERROR;
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

    clearSymtable(parser.localSymtable);
    parser.outsideBody = false;
    parser.currFunc = "main";
    stackFree(parser.undefStack);
    return err;
}

int parseMain(Scanner *scanner) {
    int err = 0;
    if (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_FUNC) {
        CHECKRULE(parseFunctionDef(scanner))
    } else if (parser.currToken.type == TYPE_EOF)
        return 0;
    else
        CHECKRULE(parseBody(scanner))

    GETTOKEN(scanner, &parser.currToken)

    CHECKRULE(parseMain(scanner))
    return err;
}

// Function to parse the end of the program
int parseProgramEnd() {
   if (parser.currToken.type == TYPE_EOF)
        return 0; // Program ends correctly
    else
        return SYNTAX_ERROR; // Unexpected token at the end
}

// Function to parse the entire program
int parseProgram(Scanner *scanner) {
    int err = 0;

    // Parse the main body of the program
    CHECKRULE(parseMain(scanner))

    // Check for the end of the program
    CHECKRULE(parseProgramEnd())

    // Return any parsing errors encountered
    return err;
}

// Entry point for parsing the input
int parse(Scanner *scanner) {
    // Initializing an empty linked list
    LinkedList empty = {.itemCount = -1};

    // Adding predefined functions to the symbol table
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

    // Getting the first token from the scanner
    GETTOKEN(scanner, &parser.currToken)

    // Starting the parsing process for the entire program
    return parseProgram(scanner);
}