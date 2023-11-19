#include "parser.h"

Parser parser;

int parserInit()
{
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
    return 0;
}

void parserDestroy()
{
    symtableFree(parser.symtable);
    symtableFree(parser.localSymtable);
    stackFree(parser.stack);
    stackFree(parser.undefStack);
    free(parser.stack);
}

int parseBody(Scanner *scanner);

// PeBody as in potentionally empty body
int parsePeBody(Scanner *scanner)
{
    int err = 0;
    // <pe_body> -> ε
    if (parser.currToken.type == TYPE_RIGHT_CURLY_BRACKET)
        return 0;
        // <pe_body> -> <body> <pe_body>.
    else
    {
        CHECKRULE(parseBody())
        GETTOKEN(scanner, &parser.currToken)
        CHECKRULE(parsePeBody())
    }

    return err;
}

// <while> -> while ( expr ) { <pe_body> }.
int parseWhile(Scanner *scanner)
{
    int err = 0;
    static int whileCnt = 0;
    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TYPE_LEFT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "While condition has to be wrapped by brackets.");
        return SYNTAX_ERROR;
    }

    whileCnt++;
    int currWhile = whileCnt;
    genWhileLoop1(currWhile);

    CHECKRULE(parseExpression(true))

    genWhileLoop2(currWhile);

    // Right bracket is checked by expression parsing

    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET)
    {
        printError(LINENUM, CHARNUM, "The body of while has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    parser.condDec = true;

    GETTOKEN(scanner, &parser.currToken)
    CHECKRULE(parsePeBody())

    parser.condDec = false;

    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET)
    {
        printError(LINENUM, CHARNUM, "The body of while has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    genWhileLoop3(currWhile);

    return err;
}

// <if> -> if ( expr ) { <pe_body> } else { <pe_body> }.
int parseIf(Scanner *scanner)
{
    int err = 0;
    static int ifCnt = 0;
    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TYPE_LEFT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "If condition has to be wrapped by brackets.");
        return SYNTAX_ERROR;
    }

    CHECKRULE(parseExpression(true))

    Token tmp = {.type = DOLLAR};
    stackPush(parser.undefStack, tmp);

    ifCnt++;
    int currentCnt = ifCnt;
    genIfElse1(currentCnt);

    // Right bracket is checked by expression parsing

    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET)
    {
        printError(LINENUM, CHARNUM, "The body of if has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    parser.condDec = true;

    GETTOKEN(scanner, &parser.currToken)
    CHECKRULE(parsePeBody())

    parser.condDec = false;

    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET)
    {
        printError(LINENUM, CHARNUM, "The body of if has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TYPE_KW || parser.currToken.value.kw != KW_ELSE)
    {
        printError(LINENUM, CHARNUM, "If has to have else clause.");
        return SYNTAX_ERROR;
    }

    genIfElse2(currentCnt);

    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET)
    {
        printError(LINENUM, CHARNUM, "The body of else has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    Token middleDelimiter = {.type = REDUCED};
    stackPush(parser.undefStack, middleDelimiter);

    parser.condDec = true;

    GETTOKEN(scanner, &parser.currToken)
    CHECKRULE(parsePeBody())

    parser.condDec = false;

    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET)
    {
        printError(LINENUM, CHARNUM, "The body of else has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    genIfElse3(currentCnt);

    return err;
}

int parseReturn(Scanner *scanner)
{
    // <return> -> return <return_p>.
    int err = 0;
    GETTOKEN(scanner, &parser.currToken)
    Keyword returning = KW_NIL;
    if (strcmp(parser.currFunc, "main") != 0)
    {
        SymtablePair *func = symtableFind(parser.symtable, parser.currFunc);
        if (func->data.parameters.itemCount != -1)
            returning = func->data.parameters.last->type;
    }

    bool expr;
    BEGINNINGOFEX()

    // <return_p>  -> expr.
    if ((!parser.outsideBody || (parser.outsideBody && returning != KW_NIL)) && expr)
    {
        CHECKRULE(parseExpression(false))
        genReturn(parser.currFunc, true);
    }
    else
    {
        if ((expr && parser.outsideBody) || (returning == KW_VOID && expr))
        {
            printError(LINENUM, CHARNUM, "Current function doesn't have a return value, so return has to be followed by a semicolon.");
            return SYNTAX_ERROR;
        }
        genReturn(parser.currFunc, false);
    }

    // <return_p>  -> ε
    return err;
}

// <params_cnp> -> lit
// <params_cnp> -> identifier_var
int parseParamsCallN(Scanner *scanner, int *pc)
{
    int err = 0;
    switch (parser.currToken.type)
    {
        case TYPE_STRING:
        case TYPE_INT:
        case TYPE_DOUBLE:
            genStackPush(parser.currToken);
            ++(*pc);
            break;

        case TOKEN_IDENTIFIER_VAR:
            if (symtableFind(parser.outsideBody ? parser.localSymtable : parser.symtable, parser.currToken.value.string.content) == NULL)
            {
                vStrFree(&(parser.currToken.value.string));
                printError(LINENUM, CHARNUM, "Passing an undefined var to a function.");
                return ERR_UNDEF_VAR;
            }
            printf("PUSHS LF@%s\n", parser.currToken.value.string);
            ++(*pc);
            break;

        case TYPE_KW:
            if (parser.currToken.value.keyword == KW_NIL)
            {
                genStackPush(parser.currToken);
                ++(*pc);
                break;
            }
            else
            {
                printError(LINENUM, CHARNUM, "Function can only be called with a variable or a literal.");
                return SYNTAX_ERROR;
                break;
            }

        default:
            printError(LINENUM, CHARNUM, "Function can only be called with a variable or a literal.");
            return SYNTAX_ERROR;
            break;
    }

    GETTOKEN(scanner, &parser.currToken)
    // <params_cn> -> , <params_cnp> <params_cn>
    if (parser.currToken.type == TYPE_COMMA)
    {
        GETTOKEN(scanner, &parser.currToken)
        return parseParamsCallN(pc);
    }
        // <params_cn> -> ε
    else
        return err;
}

int parseParamsCall(int *pc)
{
    // <params_cp> -> ε
    if (parser.currToken.type == TYPE_RIGHT_BRACKET)
        return 0;
        // <params_cp> -> lit <params_cn>.
        // <params_cp> -> identifier_var <params_cn>.
    else
        return parseParamsCallN(pc);
}

int parseFunctionCall(Scanner *scanner)
{
    int err = 0;

    SymtablePair *foundFunction = symtableFind(parser.symtable, parser.currToken.value.string);

    if (foundFunction == NULL)
    {
        vStrFree(&(parser.currToken.value.string));
        printError(LINENUM, CHARNUM, "Calling an undefined function.");
        return SEMANTIC_DEFINITION_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TYPE_LEFT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "Function's parameters have to be in a bracket (opening).");
        return SYNTAX_ERROR;
    }

    // Holds the number of parameters passed to the called function
    int parametersRealCount = 0;

    GETTOKEN(scanner, &parser.currToken)
    CHECKRULE(parseParamsCall(&parametersRealCount))

    if (!parser.outsideBody)
    {
        if ((parametersRealCount != foundFunction->data.paramsCnt) && foundFunction->data.paramsCnt != -1)
        {
            printError(LINENUM, CHARNUM, "Wrong number of parameters passed");
            return ERR_FUNC_VAR;
        }
    }

    if (parser.currToken.type != TYPE_RIGHT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "Function's parameters have to be in a bracket (closing).");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)

    ListNode *rv;
    if (foundFunction->data.parameters.itemCount == -1)
    {
        rv = NULL;
    }
    else
    {
        rv = foundFunction->data.parameters.last;
    }
    genFuncCall((char *)foundFunction->key, parametersRealCount, rv);

    return err;
}

int parseAssign(Token variable)
{
    int err = 0;
    GETTOKEN(scanner, &parser.currToken)
    // <assign_v> -> <func>
    if (parser.currToken.type == TOKEN_IDENTIFIER_FUNC)
    {
        CHECKRULE(parseFunctionCall())
    }
        // <assign_v> -> expr
    else
    {
        CHECKRULE(parseExpression(false))
    }

    if (parser.condDec)
    {
        stackPush(parser.undefStack, variable);
    }
    genAssignVariable(variable);

    return err;
}

int parseBody(Scanner *scanner)
{
    int err = 0;

    if (parser.currToken.type == TYPE_KW)
        switch (parser.currToken.value.kw)
        {
            // <body> -> <if>
            case KW_IF:
                CHECKRULE(parseIf())
                break;

                // <body> -> <while>
            case KW_WHILE:
                CHECKRULE(parseWhile())
                break;

                // <body> -> <return> ;
            case KW_RETURN:
                CHECKRULE(parseReturn())
                break;

                // <body> -> expr ;
            case KW_NIL:
                CHECKRULE(parseExpression(false))
                break;

            default:
                printError(LINENUM, CHARNUM, "Unexpected keyword found.");
                return SYNTAX_ERROR;
                break;
        }
    else if (parser.currToken.type == TOKEN_IDENTIFIER_VAR)
    {
        Token nextToken;
        if (GETTOKEN(scanner, &nextToken) != 0)
            return LEXICAL_ERROR;

        // <body> -> identifier_var = <assign_v> ;
        if (nextToken.type == TYPE_EQUAL)
        {
            Token variable = parser.currToken;
            SymtablePair *alrDefined = symtableFind(parser.outsideBody ? parser.localSymtable : parser.symtable, variable.value.string.content);
            if (alrDefined == NULL)
            {
                genDefineVariable(variable);
            }
            GETTOKEN(scanner, &parser.currToken)
            CHECKRULE(parseAssign(variable))
            LinkedList empty = {.itemCount = 0};
            if (alrDefined == NULL || alrDefined->data.possiblyUndefined)
            {
                symtableAdd(parser.outsideBody ? parser.localSymtable : parser.symtable, variable.value.string.content, VAR, -1, parser.condDec, empty);
            }
        }
            // <body> -> expr ;
        else
        {
            CHECKRULE(parseExpression(false))
        }
    }
    else if (parser.currToken.type == TOKEN_IDENTIFIER_FUNC)
    {
        CHECKRULE(parseFunctionCall())
    }
//    else if (parser.currToken.type == TOKEN_SEMICOLON)
//    {
//        return SYNTAX_ERROR;
//    }
    else
    {
        // <body> -> expr ;
        CHECKRULE(parseExpression(false))
    }

    return err;
}

int parseTypeP(LinkedList *ll)
{
    if (parser.currToken.type != TYPE_KW)
    {
        printError(LINENUM, CHARNUM, "Expected variable type.");
        return SYNTAX_ERROR;
    }

    // <type_p> -> float.
    // <type_p> -> int.
    // <type_p> -> string.
    switch (parser.currToken.value.keyword)
    {
        case KW_STRING:
        case KW_INT:
        case KW_DOUBLE:
            if (ll != NULL)
            {
                listInsert(ll, parser.currToken.value.kw);
                ll->head->opt = false;
            }
            return 0;

        default:
            printError(LINENUM, CHARNUM, "Expected variable type.");
            return SYNTAX_ERROR;
    }
}

// <type_n> -> ?<type_p>.
int parseTypeN(Scanner *scanner, LinkedList *ll)
{
    int err = 0;
    GETTOKEN(scanner, &parser.currToken)
    CHECKRULE(parseTypeP(ll))
    ll->head->opt = true;
    return err;
}

int parseParamsDefN(Scanner *scanner, LinkedList *ll)
{
    int err = 0;
    if (parser.currToken.type != TOKEN_OPTIONAL_TYPE)
    {
        CHECKRULE(parseTypeP(ll))
    }
    else
    {
        CHECKRULE(parseTypeN(ll))
    }

    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TOKEN_IDENTIFIER_VAR)
    {
        printError(LINENUM, CHARNUM, "Type has to be followed by a variable.");
        return SYNTAX_ERROR;
    }

    ll->head->name = parser.currToken.value.string;

    stackPush(parser.undefStack, parser.currToken);

    LinkedList empty = {.itemCount = 0};
    symtableAdd(parser.localSymtable, parser.currToken.value.string, VAR, -1, parser.condDec, empty);

    GETTOKEN(scanner, &parser.currToken)
    // <params_dn> -> , <type_n> identifier_var <params_dn>
    if (parser.currToken.type == TYPE_COMMA)
    {
        GETTOKEN(scanner, &parser.currToken)
        return parseParamsDefN(ll);
    }
        // <params_dn>  -> ε
    else
        return err;
}

int parseParamsDef(LinkedList *ll)
{
    // <params_dp>  -> ε
    if (parser.currToken.type == TYPE_RIGHT_BRACKET)
        return 0;
        // <params_dp>  -> <type_n> identifier_var <params_dn>.
    else
        return parseParamsDefN(ll);
}

int parseType(LinkedList *ll)
{
    // <type> -> <type_n>.
    if (parser.currToken.type == TOKEN_OPTIONAL_TYPE)
    {
        return parseTypeN(ll);
    }
        // <type> -> void.
    else if (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_VOID)
    {
        listInsert(ll, parser.currToken.value.kw);
        return 0;
    }

    // <type> -> <type_p>.
    return parseTypeP(ll);
}

int findDuplicateParams()
{
    StackItem *curr = parser.undefStack->head;
    while (curr != NULL)
    {
        StackItem *next = curr->next;
        while (next != NULL)
        {
            if (strcmp(next->t.value.string.content, curr->t.value.string) == 0)
                return 1;
            next = next->next;
        }
        curr = curr->next;
    }
    return 0;
}

// <func_def>  -> function identifier_func ( <params_dp> ) : <type> { <pe_body> }.
int parseFunctionDef(Scanner *scanner)
{
    int err = 0;
    stackFree(parser.undefStack);
    parser.outsideBody = true;

    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TOKEN_IDENTIFIER_FUNC)
    {
        printError(LINENUM, CHARNUM, "Keyword function has to be followed by function identifier.");
        return SYNTAX_ERROR;
    }
    parser.currFunc = parser.currToken.value.string;

    if (symtableFind(parser.symtable, parser.currToken.value.string) != NULL)
    {
        vStrFree(&(parser.currToken.value.string));
        printError(LINENUM, CHARNUM, "Redefinition of function.");
        return SEMANTIC_DEFINITION_ERROR;
    }

    Token func = parser.currToken;

    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TYPE_LEFT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "Function definition has to have (empty) set of params.");
        return SYNTAX_ERROR;
    }

    LinkedList ll;
    listInit(&ll);
    GETTOKEN(scanner, &parser.currToken)
    CHECKRULE(parseParamsDef(&ll))

    if (parser.currToken.type != TYPE_RIGHT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "Function definition has to have (empty) set of params.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TYPE_COLON)
    {
        printError(LINENUM, CHARNUM, "Incorrect function definition syntax, missing colon.");
        return SYNTAX_ERROR;
    }

    GETTOKEN(scanner, &parser.currToken)
    CHECKRULE(parseType(&ll))

    GETTOKEN(scanner, &parser.currToken)
    if (parser.currToken.type != TYPE_LEFT_CURLY_BRACKET)
    {
        printError(LINENUM, CHARNUM, "The body of function has to be wrapped by braces (opening).");
        return SYNTAX_ERROR;
    }

    symtableAdd(parser.symtable, func.value.string.content, FUNC, ll.itemCount - 1, parser.condDec, ll);

    if (findDuplicateParams() != 0)
    {
        return SEMANTIC_TYPE_ERROR;
    }

    genFuncDef1(func.value.string.content, ll.itemCount - 1, ll);

    GETTOKEN(scanner, &parser.currToken)
    CHECKRULE(parsePeBody())

    genFuncDef2(func.value.string);

    if (parser.currToken.type != TYPE_RIGHT_CURLY_BRACKET)
    {
        printError(LINENUM, CHARNUM, "The body of function has to be wrapped by braces (closing).");
        return SYNTAX_ERROR;
    }

    parser.outsideBody = false;
    parser.currFunc = "main";
    stackFree(parser.undefStack);
    return err;
}

int parseMainBody(Scanner *scanner)
{
    int err = 0;

    // <body_main> -> <func_def> <body_main>
    if (parser.currToken.type == TYPE_KW && parser.currToken.value.kw == KW_FUNC)
        CHECKRULE(parseFunctionDef(scanner))
        // <body_main> -> ε
    else if (parser.currToken.type == TYPE_MORE || parser.currToken.type == TYPE_EOF)
        return 0;
        // <body_main> -> <body> <body_main>
    else
        CHECKRULE(parseBody(scanner))

    GETTOKEN(scanner, &parser.currToken)
    CHECKRULE(parseMainBody(scanner))
    return err;
}

int parseProgramEnd(Scanner *scanner)
{
    // <program_e> -> ε
    if (parser.currToken.type == TYPE_MORE)
    {
        GETTOKEN(scanner, &parser.currToken)
        if (parser.currToken.type == TYPE_EOF)
            return 0;
        else
        {
            printError(LINENUM, CHARNUM, "Closing tag can only be followed by EOL.");
            return SYNTAX_ERROR;
        }
    }
        // <program_e> -> ?>
    else if (parser.currToken.type == TYPE_EOF)
        return 0;
    else
        return SYNTAX_ERROR;
}

// <program> -> <body_main> <program_e>.
int parseProgram(Scanner *scanner)
{
    int err = 0;
    CHECKRULE(parseMainBody(scanner))
    CHECKRULE(parseProgramEnd(scanner))
    return err;
}

int parse(Scanner *scanner)
{

    LinkedList empty = {.itemCount = -1};
    // Insert builtin functions
    symtableAdd(parser.symtable, "readString", FUNC, 0, false, empty);
    symtableAdd(parser.symtable, "readInt", FUNC, 0, false, empty);
    symtableAdd(parser.symtable, "readDouble", FUNC, 0, false, empty);
    symtableAdd(parser.symtable, "write", FUNC, -1, false, empty);
    symtableAdd(parser.symtable, "Int2Double", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "Double2Int", FUNC, 1, false, empty);
//    symtableAdd(parser.symtable, "strval", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "length", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "substring", FUNC, 3, false, empty);
    symtableAdd(parser.symtable, "ord", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "chr", FUNC, 1, false, empty);

    GETTOKEN(scanner, &parser.currToken)
    return parseProgram(scanner);

    return 0;
}
