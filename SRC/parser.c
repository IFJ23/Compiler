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

// Destroys the parser and frees all allocated memory
void parserDestroy() {
    symtableFree(parser.symtable);
    symtableFree(parser.localSymtable);
    symtableFree(parser.ifSymtable);
    stackFree(parser.stack);
    stackFree(parser.undefStack);
    free(parser.stack);
}

// Clears the contents of the provided symbol table and reinitializes it
void clearSymtable(Symtable *symtable) {
    symtableFree(symtable);
    symtable = symtableInit(1024); // Reinitialize the symtable
}

int parseBody(Scanner *scanner);

// Parses the body inside parentheses or curly braces
// Arguments:
// - scanner: Scanner pointer used to extract tokens
// Returns:
// - Error code (if any), 0 for successful parsing
int parsePeBody(Scanner *scanner) {
    int err = 0; // Initialize error code to 0

    // If the current token is a right curly bracket, it marks the end of the body
    if (parser.currToken.type == TYPE_RIGHT_CURLY_BRACKET)
        return 0; // Exit, indicating successful parsing of the body

    else {
        CHECKRULE(parseBody(scanner)) // Parse the body
        GETTOKEN(scanner, &parser.currToken) // Get the next token

        CHECKRULE(parsePeBody(scanner)) // Recursively parse the remaining body
    }

    return err; // Return the error code
}


// Adds a variable to the appropriate symbol table based on the current parsing context
// Arguments:
// - varName: Name of the variable to be added
// - varType: Type of the variable to be added (VAR, LET, etc.)
void addVariable(const char *varName, ElemType varType) {
    Symtable *targetSymtable = NULL; // Initialize the target symbol table pointer to NULL

    // Determine the appropriate symbol table based on the parsing context
    if (parser.ifScope) {
        targetSymtable = parser.ifSymtable; // Inside an 'if' statement, use its local symbol table
    } else if (parser.outsideBody) {
        targetSymtable = parser.localSymtable; // Outside a function body, use the local symbol table
    } else {
        targetSymtable = parser.symtable; // In global context, use the global symbol table
    }

    LinkedList empty = {.itemCount = 0}; // Create an empty linked list
    // Add the variable to the determined symbol table with relevant information
    symtableAdd(targetSymtable, varName, varType, -1, parser.condDec, empty);
}

// Finds a variable in the symbol tables based on the variable name
// Arguments:
// - varName: Name of the variable to be found
// Returns:
// - Pointer to the SymtablePair containing the variable information, or NULL if not found
SymtablePair *findVariable(const char *varName) {
    SymtablePair *foundVar = NULL; // Initialize the foundVar pointer to NULL

    // Check conditions based on the scope and if statements
    if (parser.outsideBody && parser.ifScope) {
        // Search in if statement's local symbol table, then in the local and global symbol tables
        foundVar = symtableFind(parser.ifSymtable, varName);
        if (foundVar == NULL) {
            foundVar = symtableFind(parser.localSymtable, varName);
            if (foundVar == NULL) {
                foundVar = symtableFind(parser.symtable, varName);
            }
        }
    } else if (parser.outsideBody) {
        // Search in the local and global symbol tables
        foundVar = symtableFind(parser.localSymtable, varName);
        if (foundVar == NULL) {
            foundVar = symtableFind(parser.symtable, varName);
        }
    } else if (parser.ifScope) {
        // Search in the if statement's local symbol table and the global symbol table
        foundVar = symtableFind(parser.ifSymtable, varName);
        if (foundVar == NULL) {
            foundVar = symtableFind(parser.symtable, varName);
        }
    } else {
        // Search only in the global symbol table
        foundVar = symtableFind(parser.symtable, varName);
    }

    return foundVar; // Return the pointer to the found variable or NULL if not found
}

// Parses the 'while' loop statement
// Arguments:
// - scanner: Pointer to the scanner
// Returns:
// - 0 if successful, otherwise returns an error based on parsing the 'while' loop statement
int parseWhile(Scanner *scanner) {
    int err = 0;
    static int whileCnt = 0; // Static counter for 'while' loops

    GETTOKEN(scanner, &parser.currToken) // Get the next token from the scanner

    whileCnt++; // Increment 'while' counter
    int currWhile = whileCnt; // Store the current 'while' counter value

    // Generate code for the beginning of the 'while' loop
    genWhileLoop1(currWhile);
    // Check if the 'while' condition starts with a left bracket
    if (parser.currToken.type == TYPE_LEFT_BRACKET) {
        CHECKRULE(parseExpression(scanner, true)) // Parse the expression for 'while' condition
    } else {
        CHECKRULE(parseExpression(scanner, false)) // Parse the expression for 'while' condition
    }
    genWhileLoop2(currWhile);

    // Check if the body of 'while' starts with a left curly bracket
    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET) {
        printError(LINENUM, "The body of while has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    parser.condDec = true; // Set condition declaration to true for the 'while' scope

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    CHECKRULE(parsePeBody(scanner)) // Parse the 'while' loop body

    parser.condDec = false; // Reset condition declaration flag

    // Check if the 'while' body ends with a right curly bracket
    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET) {
        printError(LINENUM, "The body of while has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    // Generate code for the end of the 'while' loop
    genWhileLoop3(currWhile);

    return err; // Return the error code or success flag
}

// Parses the 'if' statement
// Arguments:
// - scanner: Pointer to the scanner
// Returns:
// - 0 if successful, otherwise returns error based on parsing the 'if' statement
int parseIf(Scanner *scanner) {
    int err = 0;

    static int ifCnt = 0; // Static counter for 'if' statements

    GETTOKEN(scanner, &parser.currToken) // Get the next token from the scanner

    // Check if the 'if' statement starts with 'let'
    if (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_LET) {
        GETTOKEN(scanner, &parser.currToken)
        if (parser.currToken.type != TYPE_IDENTIFIER_VAR) {
            printError(LINENUM, "Let has to be followed by a variable.");
            return SYNTAX_ERROR;
        }

        // Check if the variable in 'let' is defined
        if (findVariable(parser.currToken.value.string) == NULL) {
            printError(LINENUM, "Passing an undefined let to the if.");
            return SEMANTIC_UNDEFINED_ERROR;
        }

        GETTOKEN(scanner, &parser.currToken)
    } else {
        // Parse the expression for 'if' condition
        if (parser.currToken.type == TYPE_LEFT_BRACKET) {
            CHECKRULE(parseExpression(scanner, true));
        } else {
            CHECKRULE(parseExpression(scanner, false));
        }
    }

    // Push a special token to the undefined stack for conditional checks
    Token tmp = {.type = DOLLAR};
    stackPush(parser.undefStack, tmp);

    ifCnt++; // Increment 'if' counter
    int currentCnt = ifCnt; // Store the current 'if' counter value
    genIfElse1(currentCnt); // Generate code for the 'if' condition

    // Check if the body of 'if' starts with a left curly bracket
    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET) {
        printError(LINENUM, "The body of if has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    parser.condDec = true; // Set condition declaration to true for the 'if' scope
    parser.ifScope = true; // Set 'if' scope flag to true

    GETTOKEN(scanner, &parser.currToken) // Get the next token
    stackFree(parser.undefStack); // Free the undefined stack

    CHECKRULE(parsePeBody(scanner)) // Parse the 'if' body

    stackFree(parser.undefStack); // Free the undefined stack

    clearSymtable(parser.ifSymtable); // Clear the symbol table for the 'if' scope
    parser.ifScope = false; // Reset the 'if' scope flag
    parser.condDec = false; // Reset condition declaration flag

    // Check if the 'if' body ends with a right curly bracket
    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET) {
        printError(LINENUM, "The body of if has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    // Check for the presence of an 'else' clause after 'if'
    if (parser.currToken.type != TYPE_KW || parser.currToken.value.kw != KW_ELSE) {
        printError(LINENUM, "If has to have an else clause.");
        return SYNTAX_ERROR;
    }

    genIfElse2(currentCnt); // Generate code for 'else' part of the 'if' statement

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    // Check if the body of 'else' starts with a left curly bracket
    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET) {
        printError(LINENUM, "The body of else has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    Token middleDelimiter = {.type = REDUCED};
    stackPush(parser.undefStack, middleDelimiter); // Push a delimiter to the undefined stack

    parser.condDec = true; // Set condition declaration to true for the 'else' scope
    parser.ifScope = true; // Set 'if' scope flag to true
    stackFree(parser.undefStack); // Free the undefined stack

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    CHECKRULE(parsePeBody(scanner)) // Parse the 'else' body

    clearSymtable(parser.ifSymtable); // Clear the symbol table for the 'else' scope
    stackFree(parser.undefStack); // Free the undefined stack
    parser.ifScope = false; // Reset the 'if' scope flag
    parser.condDec = false; // Reset condition declaration flag

    // Check if the 'else' body ends with a right curly bracket
    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET) {
        printError(LINENUM, "The body of else has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    genIfElse3(currentCnt); // Generate code for the end of 'if-else'

    return err; // Return the error code or success flag
}

// Parses the 'return' statement
// Arguments:
// - scanner: Pointer to the scanner
// Returns:
// - 0 if successful, otherwise returns error based on parsing return statement
int parseReturn(Scanner *scanner) {
    int err = 0;

    // Get the next token from the scanner
    GETTOKEN(scanner, &parser.currToken)

    // Default returning type set to KW_NIL
    Keyword returning = KW_NIL;

    // Check if the current function is not 'main', then retrieve the function details from the symbol table
    if (strcmp(parser.currFunc, "main") != 0) {
        SymtablePair *func = symtableFind(parser.symtable, parser.currFunc);
        // If the function's parameter count is defined (-1 denotes unknown parameters), set the returning type
        if (func->value.parameters.itemCount != -1)
            returning = func->value.parameters.active->type;
    }

    // Check conditions for valid return statements
    if ((!parser.outsideBody || (parser.outsideBody && returning != KW_NIL))) {
        // Parse the expression following the 'return' statement and generate return code
        CHECKRULE(parseExpression(scanner, false))
        genReturn(parser.currFunc, true); // Generate return code with return value
    } else {
        // Handle cases when the 'return' statement is invalid
        if ((parser.outsideBody) || (returning == KW_VOID)) {
            printError(LINENUM, "Current function doesn't have a return value");
            return SYNTAX_ERROR;
        }
        genReturn(parser.currFunc, false); // Generate return code without a return value
    }

    return err; // Return the error code or success flag
}

// Parses next function call parameters
// Arguments:
// - scanner: Pointer to the scanner
// - pc: Pointer to the integer storing parameter count
// - foundFunction: Pointer to the found function in the symbol table
// Returns:
// - 0 if successful, otherwise returns error based on parsing parameters
int parseParamsCallNext(Scanner *scanner, int *pc, SymtablePair *foundFunction) {
    int err = 0;

    // Check if the found function allows variable parameters or has unknown parameters
    if (foundFunction->value.paramsCnt == -1 || foundFunction->value.parameters.itemCount == -1) {
        // Handling variable or unknown parameters
        switch (parser.currToken.type) {
            // Cases for different token types encountered during parsing
            case TYPE_STRING:
            case TYPE_INT:
            case TYPE_DOUBLE:
                // Generating code for literal types and incrementing parameter count
                genStackPush(parser.currToken);
                ++(*pc);
                break;
            case TYPE_IDENTIFIER_VAR:
                // Checking if the variable exists in the symbol table
                if (findVariable(parser.currToken.value.string) == NULL) {
                    printError(LINENUM, "Passing an undefined var to a function.");
                    return SEMANTIC_UNDEFINED_ERROR;
                }
                // Generating code for the variable and incrementing parameter count
                printf("PUSHS LF@%s\n", parser.currToken.value.string);
                ++(*pc);
                break;
            case TYPE_KW:
                // Handling KW_NIL case
                if (parser.currToken.value.kw == KW_NIL) {
                    genStackPush(parser.currToken);
                    ++(*pc);
                    break;
                } else {
                    // Error for invalid token types
                    printError(LINENUM, "Function can only be called with a variable or a literal.");
                    return SYNTAX_ERROR;
                    break;
                }
            default:
                // Default error for unexpected token types
                printError(LINENUM, "Function can only be called with a variable or a literal.");
                return SYNTAX_ERROR;
                break;
        }
    } else {
        // Handling defined parameters
        LinkedList params = foundFunction->value.parameters;
        ListNode *prop = listGetByIndex(&params, *pc);
        if (prop && strcmp(prop->name, "_") != 0) {
            // Checking if the parameter names match
            if (strcmp(prop->name, parser.currToken.value.string) != 0) {
                printError(LINENUM, "Name is wrong.");
                return SEMANTIC_TYPE_ERROR;
            } else {
                // Moving to the next token after the parameter name
                GETTOKEN(scanner, &parser.currToken)
                if (parser.currToken.type != TYPE_COLON) {
                    printError(LINENUM, "Expected ':' after variable name.");
                    return SYNTAX_ERROR;
                }
                GETTOKEN(scanner, &parser.currToken)
            }
        }
        // Handling various token types for parameters and incrementing parameter count
        switch (parser.currToken.type) {
            // Cases for different token types encountered during parsing
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

    // Move to the next token after parsing the current parameter
    GETTOKEN(scanner, &parser.currToken)

    // Check if there's a comma and parse the next parameter recursively or return
    if (parser.currToken.type == TYPE_COMMA) {
        GETTOKEN(scanner, &parser.currToken)
        return parseParamsCallNext(scanner, pc, foundFunction);
    } else
        return err;
}

// Parses function call parameters
// Arguments:
// - scanner: Pointer to the scanner
// - pc: Pointer to the integer storing parameter count
// - foundFunction: Pointer to the found function in the symbol table
// Returns:
// - 0 if successful, else returns the result of parsing function call parameters
int parseParamsCall(Scanner *scanner, int *pc, SymtablePair *foundFunction) {
    // If the current token is a right bracket, no parameters to parse, return success
    if (parser.currToken.type == TYPE_RIGHT_BRACKET)
        return 0;
    else
        // Otherwise, parse the function call parameters and return the result
        return parseParamsCallNext(scanner, pc, foundFunction);
}

// Parses and handles a function call
// Arguments:
// - scanner: Pointer to the scanner
// Returns:
// - 0 if successful, else returns an error code
int parseFunctionCall(Scanner *scanner) {
    int err = 0;

    // Find the function in the symbol table
    SymtablePair *foundFunction = symtableFind(parser.symtable, parser.currToken.value.string);

    // If function not found, throw an error
    if (foundFunction == NULL) {
        printError(LINENUM, "Calling an undefined function.");
        return SEMANTIC_DEFINITION_ERROR;
    }

    // Move to the next token
    GETTOKEN(scanner, &parser.currToken)

    // Check for an opening bracket for function parameters
    if (parser.currToken.type != TYPE_LEFT_BRACKET) {
        printError(LINENUM, "Function's parameters have to be in a bracket (opening).");
        return SYNTAX_ERROR;
    }

    int parametersRealCount = 0;

    // Move to the next token
    GETTOKEN(scanner, &parser.currToken)

    // Parse the parameters for the function call
    CHECKRULE(parseParamsCall(scanner, &parametersRealCount, foundFunction))

    // If not outside the function body, check the number of parameters passed
    if (!parser.outsideBody) {
        if ((parametersRealCount != foundFunction->value.paramsCnt) && foundFunction->value.paramsCnt != -1) {
            printError(LINENUM, "Wrong number of parameters passed");
            return SEMANTIC_TYPE_ERROR;
        }
    }

    // Check for a closing bracket for function parameters
    if (parser.currToken.type != TYPE_RIGHT_BRACKET) {
        printError(LINENUM, "Function's parameters have to be in a bracket (closing).");
        return SYNTAX_ERROR;
    }

    // Move to the next token
    GETTOKEN(scanner, &parser.currToken)

    // Get the ListNode for parameters or set it to NULL if the count is -1
    ListNode *rv;
    if (foundFunction->value.parameters.itemCount == -1) {
        rv = NULL;
    } else {
        rv = foundFunction->value.parameters.active;
    }

    // Generate the function call
    genFuncCall((char *) foundFunction->key, parametersRealCount, rv);

    return err;
}

// Parses and handles variable assignment
// Arguments:
// - scanner: Pointer to the scanner
// - variable: Token representing the variable being assigned
// Returns:
// - 0 if successful, else returns an error code
int parseAssign(Scanner *scanner, Token variable) {
    int err = 0;

    GETTOKEN(scanner, &parser.currToken)

    // Check the type of the next token for different assignment scenarios
    if (parser.currToken.type == TYPE_IDENTIFIER_FUNC) {
        CHECKRULE(parseFunctionCall(scanner))
    } else if (parser.currToken.type == TYPE_LEFT_BRACKET ||
               parser.currToken.type == TYPE_IDENTIFIER_VAR ||
               parser.currToken.type == TYPE_INT ||
               parser.currToken.type == TYPE_DOUBLE ||
               (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_NIL) ||
               parser.currToken.type == TYPE_MULTILINE_STRING ||
               parser.currToken.type == TYPE_STRING) {
        // For valid token types, parse the expression
        CHECKRULE(parseExpression(scanner, false))
    } else {
        return SYNTAX_ERROR; // Unexpected token type for assignment
    }

    // If conditional declaration is true, push the variable to the undefined stack
    if (parser.condDec) {
        stackPush(parser.undefStack, variable);
    }

    // Generate the assignment operation
    genAssignVariable(variable);

    return err;
}

// Parses a variable type and updates the linked list with type information
// Arguments:
// - ll: Pointer to the linked list to update with type information
// - name: Name of the variable
// Returns:
// - 0 if successful, else returns an error code
int parseVariableType(LinkedList *ll, char *name) {
    if (parser.currToken.type != TYPE_KW) {
        printError(LINENUM, "Expected variable type.");
        return SYNTAX_ERROR;
    }

    switch (parser.currToken.value.kw) {
        // Handle predefined types: string, int, double
        case KW_STRING:
        case KW_INT:
        case KW_DOUBLE:
            if (ll != NULL) {
                // Insert type information into the linked list
                listInsert(ll, parser.currToken.value.kw, name);
                ll->head->opt = false; // Set optional flag to false
            }
            return 0;

            // Handle undefined types: undefined_string, undefined_int, undefined_double
        case KW_UNDEFINED_DOUBLE:
        case KW_UNDEFINED_INT:
        case KW_UNDEFINED_STRING:
            if (ll != NULL) {
                // Insert type information into the linked list
                listInsert(ll, parser.currToken.value.kw, name);
                ll->head->opt = true; // Set optional flag to true
            }
            return 0;

        default:
            printError(LINENUM, "Expected variable type.");
            return SYNTAX_ERROR;
    }
}

// Parses the redefinition of a variable
// Arguments:
// - scanner: Pointer to the scanner object
// - variable: Token representing the variable being redefined
// Returns:
// - 0 if successful, else returns an error code
int parseVariableRedefinition(Scanner *scanner, Token variable) {
    int err = 0;

    // Find the variable in the symbol table
    SymtablePair *alrDefined = findVariable(variable.value.string);

    // Check if the variable is defined as a constant (LET), disallow redefinition
    if (alrDefined && alrDefined->value.type == LET) {
        printError(LINENUM, "It is not allowed to redefine a constant.");
        return SEMANTIC_DEFINITION_ERROR;
    } else if (alrDefined && alrDefined->value.type == VAR) {
        // If the variable is already defined as a variable (VAR), handle redefinition

        // Check for syntax error if the next token is not an assignment
        if (peek_token(scanner, &parser.currToken) != 0)
            return LEXICAL_ERROR;

        if (parser.currToken.type == TYPE_ASSIGN) {
            // If the next token is an assignment, parse the assignment

            // Generate variable definition if not already defined
            if (alrDefined == NULL) {
                genDefineVariable(variable);
            }

            // Parse assignment for the variable
            GETTOKEN(scanner, &parser.currToken)
            CHECKRULE(parseAssign(scanner, variable))

            // If the variable was previously undefined or possibly undefined, mark it as defined
            if (alrDefined == NULL || alrDefined->value.possiblyUndefined) {
                addVariable(variable.value.string, VAR);
            }
        } else {
            // Handle syntax error if an assignment is expected after the variable name
            printError(LINENUM, "Syntax Error");
            return SYNTAX_ERROR;
        }
    } else {
        // If the variable is not found in the symbol table, it's undefined
        printError(LINENUM, "Undefined variable used in an expression.");
        return SEMANTIC_UNDEFINED_ERROR;
    }

    return err;
}

// Parses the body of a program or function
// Arguments:
// - scanner: Pointer to the scanner object
// Returns:
// - 0 if successful, else returns an error code
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

                // Parsing variable declarations
                // Checking for redefinition of variables
                // Parsing variable definitions and assignments
                // Handling 'var' keyword
                // Checking if it's a new variable or redefinition
                // Parsing the type of variable
                // Checking if the variable is possibly undefined and assigning values
                // Parsing assignment if present, else returning successfully
                // Handling unexpected tokens

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

                        CHECKRULE(parseVariableType(&ll, variable.value.string))

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
                // Parsing 'let' keyword
                // Parsing variable declarations
                // Checking for redefinition of variables
                // Parsing variable definitions and assignments
                // Checking if it's a new variable or redefinition
                // Parsing the type of variable
                // Checking if the variable is possibly undefined and assigning values
                // Parsing assignment if present, else parsing expression

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

                        CHECKRULE(parseVariableType(&ll, variable.value.string))

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
                // Parsing 'nil' keyword
                // Parsing expression
                CHECKRULE(parseExpression(scanner, false))
                break;

            default:
                printError(LINENUM, "Unexpected keyword found.");
                return SYNTAX_ERROR;
        }
    else if (parser.currToken.type == TYPE_IDENTIFIER_FUNC) {
        CHECKRULE(parseFunctionCall(scanner)) // Parsing function calls
    } else if (parser.currToken.type == TYPE_IDENTIFIER_VAR) {
        CHECKRULE(parseVariableRedefinition(scanner, parser.currToken)) // Parsing variable redefinition
    } else if (parser.currToken.type == TYPE_RETURN_ARROW) {
        return SYNTAX_ERROR;
    } else if (parser.currToken.type == TYPE_EOL) {
        return 0; // Successfully parsed end of line
    } else {
        CHECKRULE(parseExpression(scanner, false)) // Parsing expressions
    }

    return err;
}

// Parses subsequent parameters in a parameter list and updates the linked list accordingly
// Arguments:
// - scanner: Pointer to the scanner object
// - ll: Pointer to the linked list where parameter information is stored
// Returns:
// - 0 if successful, else returns an error code
int parseParamsDefNext(Scanner *scanner, LinkedList *ll) {
    int err = 0;

    Token name = parser.currToken;
    Token id;

    // Check if the current token denotes an identifier for a variable
    if (parser.currToken.type != TYPE_IDENTIFIER_VAR) {
        printError(LINENUM, "Type has to be followed by a variable.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    // Validate if the current token denotes an identifier for a variable
    if (parser.currToken.type != TYPE_IDENTIFIER_VAR) {
        printError(LINENUM, "Type has to be followed by a variable.");
        return SYNTAX_ERROR;
    }

    // Ensure that the name and ID are different
    if (strcmp(parser.currToken.value.string, name.value.string) == 0) {
        printError(LINENUM, "Name and ID should be different.");
        return SEMANTIC_TYPE_ERROR;
    }

    // If the current token denotes an identifier for a variable, assign it to `id`
    if (parser.currToken.type == TYPE_IDENTIFIER_VAR) {
        id = parser.currToken;
        GETTOKEN(scanner, &parser.currToken)
    }

    // Check if the current token denotes a colon (type specifier)
    if (parser.currToken.type != TYPE_COLON) {
        printError(LINENUM, "Expected ':' after the variable name.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    // Parse the type specifier and update the linked list
    CHECKRULE(parseVariableType(ll, name.value.string))

    // Push the identifier onto the undefined stack
    stackPush(parser.undefStack, id);

    // Add the variable to the local symbol table
    LinkedList empty = {.itemCount = 0};
    symtableAdd(parser.localSymtable, id.value.string, VAR, -1, parser.condDec, empty);

    GETTOKEN(scanner, &parser.currToken)

    // If the current token denotes a comma, continue parsing subsequent parameters
    if (parser.currToken.type == TYPE_COMMA) {
        GETTOKEN(scanner, &parser.currToken)

        return parseParamsDefNext(scanner, ll);
    } else {
        return err; // No more parameters to parse, return 0
    }
}

// Parses parameter definitions and updates the linked list accordingly
// Arguments:
// - scanner: Pointer to the scanner object
// - ll: Pointer to the linked list where parameter information is stored
// Returns:
// - 0 if no parameters are found, else returns the result of `parseParamsDefN`
int parseParamsDef(Scanner *scanner, LinkedList *ll) {
    // Check if the current token denotes the end of parameter definitions
    if (parser.currToken.type == TYPE_RIGHT_BRACKET)
        return 0; // No parameters found, return 0

    // Continue parsing the parameters
    return parseParamsDefNext(scanner, ll); // Proceed to parse parameter definitions
}

// Parses the type declaration and updates the linked list accordingly
// Arguments:
// - ll: Pointer to the linked list where type information is stored
// Returns:
// - 0 on successful parsing, appropriate error code otherwise
int parseType(LinkedList *ll) {
    // Check if the current token represents the 'void' keyword
    if (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_VOID) {
        // Insert 'void' keyword into the linked list for the 'return' type
        listInsert(ll, parser.currToken.value.kw, "return");
        return 0; // Successful parsing of 'void' type
    }
    // If not 'void', proceed to parse the type further
    return parseVariableType(ll, "return"); // Continue parsing the type
}

// Function to find duplicate parameters in the stack
int checkForDuplicateParams() {
    // Start from the head of the undefined stack
    StackItem *curr = parser.undefStack->head;

    // Iterate through the stack until the end
    while (curr != NULL) {
        StackItem *next = curr->next;

        // Iterate through subsequent elements
        while (next != NULL) {
            // Check if the parameter names match
            if (strcmp(next->t.value.string, curr->t.value.string) == 0)
                return 1; // If a duplicate is found, return 1
            next = next->next;
        }
        curr = curr->next;
    }
    return 0; // Return 0 if no duplicates are found
}

// Parsing function for defining a function
int parseFunctionDef(Scanner *scanner) {
    int err = 0;

    // Free the undefined stack and set flag for being outside of a function body
    stackFree(parser.undefStack);
    parser.outsideBody = true;

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    // Check if the next token is a function identifier
    if (parser.currToken.type != TYPE_IDENTIFIER_FUNC) {
        printError(LINENUM, "Keyword function has to be followed by function identifier.");
        return SYNTAX_ERROR;
    }
    parser.currFunc = parser.currToken.value.string; // Set the current function identifier

    // Check if the function has already been defined
    if (symtableFind(parser.symtable, parser.currToken.value.string) != NULL) {
        printError(LINENUM, "Redefinition of function.");
        return SEMANTIC_DEFINITION_ERROR;
    }

    Token func = parser.currToken; // Store the function identifier token

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    // Check if the function definition has an empty set of parameters
    if (parser.currToken.type != TYPE_LEFT_BRACKET) {
        printError(LINENUM, "Function definition has to have (empty) set of params.");
        return SYNTAX_ERROR;
    }

    LinkedList ll;
    listInit(&ll); // Initialize a linked list for parameters
    GETTOKEN(scanner, &parser.currToken) // Get the next token

    CHECKRULE(parseParamsDef(scanner, &ll)) // Parse parameters

    // Check if the function definition has an empty set of parameters
    if (parser.currToken.type != TYPE_RIGHT_BRACKET) {
        printError(LINENUM, "Function definition has to have (empty) set of params.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    // Check if the function definition has the return arrow
    if (parser.currToken.type != TYPE_RETURN_ARROW) {
        printError(LINENUM, "Incorrect function definition syntax, missing colon.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    CHECKRULE(parseType(&ll)) // Parse the return type

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    // Check if the function body is wrapped by opening braces
    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET) {
        printError(LINENUM, "The body of function has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    // Add the function to the symbol table
    symtableAdd(parser.symtable, func.value.string, FUNC, ll.itemCount - 1, parser.condDec, ll);

    // Check for duplicate parameters in the function definition
    if (checkForDuplicateParams() != 0) {
        return SEMANTIC_TYPE_ERROR;
    }

    // Generate function definition 1
    genFuncDef1(func.value.string, ll.itemCount - 1, ll);

    GETTOKEN(scanner, &parser.currToken) // Get the next token

    CHECKRULE(parsePeBody(scanner)) // Parse the function body

    // Generate function definition 2
    genFuncDef2(func.value.string);

    // Check if the function body is wrapped by closing braces
    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET) {
        printError(LINENUM, "The body of function has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    // Clear the local symbol table, reset flags, and free memory
    clearSymtable(parser.localSymtable);
    parser.outsideBody = false;
    parser.currFunc = "main";
    stackFree(parser.undefStack);

    return err; // Return any accumulated errors
}


// Main parsing function for the program
int parseMain(Scanner *scanner) {
    int err = 0;
    // Check if the current token is a function definition
    if (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_FUNC) {
        CHECKRULE(parseFunctionDef(scanner)) // Parse a function definition
    } else if (parser.currToken.type == TYPE_EOF) {
        return 0; // End of file, parsing complete
    } else {
        CHECKRULE(parseBody(scanner)) // Parse the program body
    }

    GETTOKEN(scanner, &parser.currToken) // Move to the next token

    CHECKRULE(parseMain(scanner)) // Continue parsing

    return err; // Return any accumulated errors
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