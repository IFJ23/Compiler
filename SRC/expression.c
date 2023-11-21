/**
 * @file expression.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Expression LR parser.
 */

#include "expression.h"
#include "codegen.h"

extern Parser parser;

int prec_table[9][9] = {
        //+, *, id, $, <=, (, ), !, ??
        {R, S,  S, R,  R, S, R, S, R}, // +
        {R, R,  S, R,  R, S, R, S, R}, // *
        {R, R,  F, R,  R, F, R, R, R}, // id
        {S, S,  S, O,  S, S, F, S, S}, // $
        {S, S,  S, R,  F, S, R, S, R}, // <=
        {S, S,  S, F,  S, S, E, S, S}, // (
        {R, R,  F, R,  R, F, R, R, R}  // ),
        {R, R,  R, R,  R, R, R, R, R}  // !,
        {S, S,  S, R,  S, S, S, S, R}  // ??,
};

Token topmostTerminal()
{
    StackItem *tmp = parser.stack->head;

    while (tmp != NULL)
    {
        if (tmp->t.type < 501)
            return tmp->t;
        else
            tmp = tmp->next;
    }

    Token err = {.type = 999};

    return err;
}

int reduceI()
{
    Token head = parser.stack->head->t;
    SymtablePair *foundVar;
    if (head.type == TYPE_ID)
    {
        foundVar = symtableFind(parser.outsideBody ? parser.localSymtable : parser.symtable, head.value.string.content);
        if (foundVar == NULL)
        {
            vStrFree(&(head.value.string));
            printError(head.pos.line, head.pos.character, "Undefined variable used in an expression.");
            return SEMANTIC_UNDEFINED_ERROR;
        }
    }

    Token t;
    stackPop(parser.stack, &t);
    if (t.type != TOKEN_IDENTIFIER_VAR)
        genStackPush(t);
    else
    {
        if (foundVar->data.possiblyUndefined)
            genCheckDefined(t);
        genStackPush(t);
    }
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }
    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
};

int reducePlus()
{

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    genStackPush(t);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceMultiply()
{
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    genStackPush(t);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceRelation()
{

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    genStackPush(t);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceBracket()
{
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_SYNTAX_AN;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceNotNil()
{
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    genStackPush(t);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceVarOrNil()
{
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_SYNTAX_AN;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

tableIndex getTableIndex(Token t)
{
    switch (t.type)
    {
        case TYPE_PLUS:
        case TYPE_MINUS:
            return I_PLUS;
        case TYPE_MUL:
        case TYPE_DIV:
            return I_MULTIPLY;
        case TYPE_STRING:
        case TYPE_INT:
        case TYPE_DOUBLE:
        case TYPE_ID:
            return I_DATA;
        case TYPE_MORE_EQUAL:
        case TYPE_MORE:
        case TYPE_LESS_EQUAL:
        case TYPE_LESS:
        case TYPE_EQUAL:
        case TYPE_NOT_EQUAL:
            return I_RELATIONAL;
        case TYPE_LEFT_BRACKET:
            return I_OPENB;
        case TYPE_RIGHT_BRACKET:
            return I_CLOSEB;
        case TYPE_EXCLAMATION_MARK:
            return I_NOTNIl;
        case TYPE_NIL_COALESCING_OPERATOR:
            return I_VALORNIL;
        case TYPE_KW:
            if (t.value.keyword == KW_NIL)
                return I_DATA;
            else
                return I_DOLLAR;
        default:
            return I_DOLLAR;
    }
}

precValues getRelation(Token top, Token new)
{
    return prec_table[getTableIndex(top)][getTableIndex(new)];
};

int reduce()
{
    switch (getTableIndex(topmostTerminal()))
    {
        case I_DATA:
            return reduceI();

        case I_PLUS:
            return reducePlus();

        case I_MULTIPLY:
            return reduceMultiply();

        case I_RELATIONAL:
            return reduceRelation();

        case I_CLOSEB:
            return reduceBracket();

        case I_NOTNIl:
            return reduceNotNil();

        case I_VALORNIL:
            return reduceVarOrNil();

        default:
            printError(0, 0, "No reduction rule for given token.");
            return ERR_INTERNAL;
    }
}

int shift(Token *preShift)
{
    Token shift = {.type = SHIFT_SYMBOL};
    Token topmost = topmostTerminal();
    if (topmost.type == 999)
    {
        printError(parser.currToken.pos.line, parser.currToken.pos.character, "Couldn't shift symbol, invalid expression.");
        return SYNTAX_ERROR;
    }
    StackItem *tmp = parser.stack->head;

    // Prepare stack to temporarily store tokens between
    // topmost nonterminal and top of the parser stack
    Stack *putaway = malloc(sizeof(Stack));
    if (putaway == NULL)
        return ERR_INTERNAL;
    stackInit(putaway);

    Token toPush;
    while (tmp->t.type != topmost.type)
    {
        stackPop(parser.stack, &toPush);
        stackPush(putaway, toPush);
        tmp = parser.stack->head;
    }

    // Push the shift symbol and return tokens to parser stack
    stackPush(parser.stack, shift);
    while (putaway->head != NULL)
    {
        stackPop(putaway, &toPush);
        stackPush(parser.stack, toPush);
    }

    free(putaway);

    stackPush(parser.stack, parser.currToken);
    *preShift = parser.currToken;
    int err = getToken(&(parser.currToken), false);

    return err;
}

int parseExpression(bool endWithBracket)
{
    int err = 0;
    Token bottom = {.type = DOLLAR};
    static Token beforeEnd = {.type = DOLLAR};
    stackPush(parser.stack, bottom);

    genExpressionBegin();

    while (true)
    {
        switch (getRelation(topmostTerminal(), parser.currToken))
        {
            case (R):
                err = reduce();
                break;

            case (S):
                err = shift(&beforeEnd);
                break;

            case (E):
                stackPush(parser.stack, parser.currToken);
                beforeEnd = parser.currToken;
                err = getToken(&(parser.currToken), false);
                break;

            case (O):
                stackFree(parser.stack);
                if (endWithBracket && beforeEnd.type != TOKEN_RIGHT_BRACKET)
                {
                    printError(beforeEnd.pos.line, beforeEnd.pos.character, "Expression has to be wrapped by braces.");
                    return 2;
                }
                genExpressionEnd();
                return 0;

            default:
                return ERR_SYNTAX_AN;
        }

        if (err != 0)
            return err;
    }
}