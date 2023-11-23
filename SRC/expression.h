// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Sviatoslav Pokhvalenko (xpokhv01)

#ifndef H_EXPRESSION
#define H_EXPRESSION

#include "symtable.h"
#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "generator.h"

#define DOLLAR 500
#define SHIFT_SYMBOL 501
#define REDUCED 502

typedef enum
{
    R, // reduce >
    S, // shift <
    E, // equal
    F, // error
    O  // ok
} precValues;

typedef enum
{
    I_PLUS,
    I_MULTIPLY,
    I_DATA,
    I_DOLLAR,
    I_RELATIONAL,
    I_OPENB,
    I_CLOSEB,
    I_NOTNIl,
    I_VALORNIL
} tableIndex;

int parseExpression(Scanner *scanner, bool endWithBracket);

#endif
