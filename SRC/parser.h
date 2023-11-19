#ifndef H_PARSER
#define H_PARSER

#include "scanner.h"
#include "structures.h"
#include "error.h"
#include <stdlib.h>
#include "expression.h"
#include "symtable.h"
#include "codegen.h"

typedef struct
{
    Token currToken;
    bool outsideBody;
    char *currFunc;
    bool condDec; // Indicates conditional declaration
    Stack *stack;
    Stack *undefStack;
    Symtable *symtable;
    Symtable *localSymtable;
} Parser;

#define LINENUM parser.currToken.line
#define CHARNUM parser.currToken.character
#define GETTOKEN(scanner, t)              \
    if (getToken(scanner, t) != 0) \
        return LEXICAL_ERROR;
#define CHECKRULE(r)    \
    do                  \
    {                   \
        err = (r);      \
        if (err != 0)   \
            return err; \
    } while (0);
#define BEGINNINGOFEX()                                    \
    do                                                     \
    {                                                      \
        switch (parser.currToken.type)                     \
        {                                                  \
        case TYPE_LEFT_BRACKET:                           \
        case TYPE_STRING:                                 \
        case TYPE_INT:                                    \
        case TYPE_DOUBLE:                                  \
        case TOKEN_IDENTIFIER_VAR:                         \
            expr = true;                                   \
            break;                                         \
        case TYPE_KW:                                \
            if (parser.currToken.value.kw == KW_NIL) \
                expr = true;                               \
            else                                           \
                expr = false;                              \
            break;                                         \
        default:                                           \
            expr = false;                                  \
            break;                                         \
        }                                                  \
    } while (0);

/**
 * @brief Initializes all structures, that need to by malloced.
 *
 * @return int Non-zero number if any of the mallocs fail, zero otherwise.
 */
int parserInit();

/**
 * @brief Frees all malloced structures used in parser global file.
 *
 */
void parserDestroy();

/**
 * @brief Initalizes parsing.
 *
 * @return int Zero if code was parsed without an error, non-zero otherwise.
 */
int parse();

/**
 * @brief Checks the "declare(strict_types=1);" part of prologue.
 *
 * @return int Non-zero number if prologue isn't in the specified format, zero otherwise.
 */
int checkPrologue();

#endif
