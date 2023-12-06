// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file expression.c
 * @brief Expression parsing for the IFJ23 language compiler
 * @author Sviatoslav Pokhvalenko (xpokhv01)
 */
#include "expression.h"

extern Parser parser;

/**
 * @brief Precedence table for the parser.
 */
int prec_table[9][9] = {
   //+, *, i, $, <=, (, ), !, ??
    {R, S, S, R,  R, S, R, S, R}, // +
    {R, R, S, R,  R, S, R, S, R}, // *
    {R, R, F, R,  R, F, R, R, R}, // i
    {S, S, S, O,  S, S, F, S, S}, // $
    {S, S, S, R,  F, S, R, S, R}, // <=
    {S, S, S, F,  S, S, E, F, S}, // (
    {R, R, F, R,  R, F, R, R, R}, // )
    {R, R, F, O,  S, F, E, E, R}, // !
    {S, S, S, R,  S, S, R, S, S}  // ??
};

/**
 * @brief Function to get the topmost terminal from the stack.
 * @return The topmost terminal token.
 */
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

/**
 * @brief Function to reduce an identifier.
 * @return 0 if successful, non-zero otherwise.
 */
int reduceI()
{
    Token head = parser.stack->head->t;
    SymtablePair *foundVar;
    if (head.type == TYPE_IDENTIFIER_VAR)
    {
        foundVar = findVariable(head.value.string);
        if (foundVar == NULL)
        {
            printError(head.line, "Undefined variable used in an expression. 1 \n");
            return SEMANTIC_UNDEFINED_ERROR;
        }
    }

    Token t;
    stackPop(parser.stack, &t);
    if (t.type != TYPE_IDENTIFIER_VAR)
        genStackPush(t);
    else
    {
        if (foundVar->value.possiblyUndefined)
            genCheckDefined(t);
        genStackPush(t);
    }
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }
    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

/**
 * @brief Function to reduce a plus operation.
 * @return 0 if successful, non-zero otherwise.
 */
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
        printError(0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

/**
 * @brief Function to reduce a multiply operation.
 * @return 0 if successful, non-zero otherwise.
 */
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
        printError(0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

/**
 * @brief Function to reduce a relational operation.
 * @return 0 if successful, non-zero otherwise.
 */
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
        printError(0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

/**
 * @brief Function to reduce a bracket operation.
 * @return 0 if successful, non-zero otherwise.
 */
int reduceBracket()
{
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

/**
 * @brief Function to reduce a not nil operation.
 * @return 0 if successful, non-zero otherwise.
 */
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
        printError(0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

/**
 * @brief Function to reduce a value or nil operation.
 * @return 0 if successful, non-zero otherwise.
 */
int reduceValOrNil()
{
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    genStackPush(t);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, "Reduction of expression failed.");
        return SYNTAX_ERROR;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

/**
 * @brief Function to get the index in the precedence table for a token.
 * @param t The token to get the index for.
 * @return The index in the precedence table.
 */
tableIndex getTableIndex(Token t)
{
    switch (t.type)
    {
        case TYPE_PLUS:
        case TYPE_MINUS:
            return I_PLUS;
        case TYPE_NIL_COALESCING_OPERATOR:
            return  I_VALORNIL;
        case TYPE_EXCLAMATION_MARK:
            return  I_NOTNIl;
        case TYPE_MUL:
        case TYPE_DIV:
            return I_MUL;
        case TYPE_STRING:
        case TYPE_INT:
        case TYPE_DOUBLE:
        case TYPE_IDENTIFIER_VAR:
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
        case TYPE_KW:
            if (t.value.kw == KW_NIL)
                return I_DATA;
            else
                return I_DOLLAR;
        default:
            return I_DOLLAR;
    }
}

/**
 * @brief Function to get the relation between two tokens from the precedence table.
 * @param top The top token.
 * @param new The new token.
 * @return The relation between the two tokens.
 */
precValues getRelation(Token top, Token new)
{
    return prec_table[getTableIndex(top)][getTableIndex(new)];
}

/**
 * @brief Function to reduce the topmost terminal on the stack.
 * @return 0 if successful, non-zero otherwise.
 */
int reduce()
{
    switch (getTableIndex(topmostTerminal()))
    {
        case I_DATA:
            return reduceI();

        case I_PLUS:
            return reducePlus();

        case I_MUL:
            return reduceMultiply();

        case I_RELATIONAL:
            return reduceRelation();

        case I_CLOSEB:
            return reduceBracket();

        case I_NOTNIl:
            return reduceNotNil();

        case I_VALORNIL:
            return reduceValOrNil();

        default:
            printError(0, "No reduction rule for given token.");
            return SEMANTIC_COMPATIBILITY_ERROR;
    }
}

/**
 * @brief Function to shift a token onto the stack.
 * @param scanner The scanner to get the tokens from.
 * @param preShift Pointer to the token to shift.
 * @return 0 if successful, non-zero otherwise.
 */
int shift(Scanner *scanner, Token *preShift)
{
    Token shift = {.type = SHIFT_SYMBOL};
    Token topmost = topmostTerminal();
    if (topmost.type == 999)
    {
        printError(parser.currToken.line,  "Couldn't shift symbol, invalid expression.");
        return SYNTAX_ERROR;
    }
    StackItem *tmp = parser.stack->head;

    Stack *putaway = malloc(sizeof(Stack));
    if (putaway == NULL)
        return INTERNAL_ERROR;
    stackInit(putaway);

    Token toPush;
    while (tmp->t.type != topmost.type)
    {
        stackPop(parser.stack, &toPush);
        stackPush(putaway, toPush);
        tmp = parser.stack->head;
    }

    stackPush(parser.stack, shift);
    while (putaway->head != NULL)
    {
        stackPop(putaway, &toPush);
        stackPush(parser.stack, toPush);
    }

    free(putaway);

    stackPush(parser.stack, parser.currToken);
    *preShift = parser.currToken;
    int err = get_token(scanner, &(parser.currToken));
    return err;
}

int parseExpression(Scanner *scanner, bool endWithBracket)
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
                err = shift(scanner, &beforeEnd);
                break;

            case (E):
                stackPush(parser.stack, parser.currToken);
                beforeEnd = parser.currToken;
                err = get_token(scanner, &(parser.currToken));
                break;

            case (O):
                if (endWithBracket && beforeEnd.type != TYPE_RIGHT_BRACKET)
                {
                    printError(beforeEnd.line, "Expression has to be wrapped by braces.");
                    return 2;
                }
                genExpressionEnd();
                return 0;

            default:
                return SYNTAX_ERROR;
        }

        if (err != 0)
            return err;
    }
}