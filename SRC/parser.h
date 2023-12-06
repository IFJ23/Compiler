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

/**
 * @brief Struct representing a parser.
 */
typedef struct
{
    Token currToken; ///< The current token being parsed.
    bool outsideBody; ///< Flag indicating if the parser is outside a function body.
    char *currFunc; ///< The current function being parsed.
    bool condDec; ///< Flag indicating if a conditional declaration is being parsed.
    bool ifScope; ///< Flag indicating if the parser is inside an if scope.
    Stack *stack; ///< Stack for storing tokens.
    Stack *undefStack; ///< Stack for storing undefined variables.
    Symtable *symtable; ///< Symbol table for the parser.
    Symtable *localSymtable; ///< Local symbol table for the parser.
    Symtable *ifSymtable; ///< Symbol table for if scopes.
    Symtable *prevIfSymtable; ///< Previous symbol table for if scopes.

} Parser;

#define LINENUM parser.currToken.line ///< Macro for getting the line number of the current token.

/**
 * @brief Macro for getting a token from the scanner and checking for lexical errors.
 */
#define GETTOKEN(scanner, t) \
    if (get_token(scanner, t) != 0) \
        return LEXICAL_ERROR;

/**
 * @brief Macro for checking a rule and returning an error if it fails.
 */
#define CHECKRULE(r)    \
    do                  \
    {                   \
        err = (r);      \
        if (err != 0)   \
            return err; \
    } while (0);

/**
 * @brief Function to initialize the parser.
 * @return 0 if successful, non-zero otherwise.
 */
int parserInit();

/**
 * @brief Function to destroy the parser and free the allocated memory.
 */
void parserDestroy();

/**
 * @brief Function to find a variable in the symbol table.
 * @param varName The name of the variable to find.
 * @return Pointer to the found variable, or NULL if not found.
 */
SymtablePair* findVariable(const char* varName);

/**
 * @brief Function to parse the input from a scanner.
 * @param scanner The scanner to get the tokens from.
 * @return 0 if successful, non-zero otherwise.
 */
int parse(Scanner *scanner);

#endif //IFJ23_PARSER_H