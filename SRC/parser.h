// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Sviatoslav Pokhvalenko (xpokhv01)

#ifndef H_PARSER
#define H_PARSER

#include "scanner.h"
#include "structures.h"
#include "error.h"
#include <stdlib.h>
#include "expression.h"
#include "symtable.h"
#include "generator.h"

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
#define GETTOKEN(scanner, t) \
    if (get_token(scanner, t) != 0) \
        return LEXICAL_ERROR;

#define CHECKENDOFEXPRESSION()                                                      \
    if (parser.currToken.type == TOKEN_SEMICOLON ) \
    {                                                                         \
        printError(LINENUM, "Missing semicolon after a statement."); \
        return SYNTAX_ERROR;                                                 \
    }
#define CHECKRULE(r)    \
    do                  \
    {                   \
        err = (r);      \
        if (err != 0)   \
            return err; \
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
int parse(Scanner *scanner);

/**
 * @brief Checks the "declare(strict_types=1);" part of prologue.
 *
 * @return int Non-zero number if prologue isn't in the specified format, zero otherwise.
 */
int checkPrologue();

#endif
