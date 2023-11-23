// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)

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
