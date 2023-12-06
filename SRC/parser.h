// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file parser.h
 * @brief Parser for the IFJ23 language compiler
 * @author Vsevolod Pokhvalenko (xpokhv00)
 */

#ifndef IFJ23_PARSER_H
#define IFJ23_PARSER_H

#include "scanner.h"
#include "linlist.h"
#include "symstack.h"
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
    bool condDec;
    Stack *stack;
    Stack *undefStack;
    Symtable *symtable;
    Symtable *localSymtable;
} Parser;

#define LINENUM parser.currToken.line
#define GETTOKEN(scanner, t) \
    if (get_token(scanner, t) != 0) \
        return LEXICAL_ERROR;

#define CHECKRULE(r)    \
    do                  \
    {                   \
        err = (r);      \
        if (err != 0)   \
            return err; \
    } while (0);



int parserInit();


void parserDestroy();


int parse(Scanner *scanner);


int checkPrologue();

#endif
