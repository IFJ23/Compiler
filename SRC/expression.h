// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file expression.h
 * @brief Expression parsing for the IFJ23 language compiler
 * @author Sviatoslav Pokhvalenko (xpokhv01)
 */

#ifndef IFJ23_EXPRESSION_H
#define IFJ23_EXPRESSION_H

#include "symtable.h"
#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "generator.h"

/**
 * @brief Symbol representing the end of the input in the precedence table.
 */
#define DOLLAR 500

/**
 * @brief Symbol representing a shift operation in the precedence table.
 */
#define SHIFT_SYMBOL 501

/**
 * @brief Symbol representing a reduced operation in the precedence table.
 */
#define REDUCED 502

/**
 * @brief Enum representing the values in the precedence table.
 */
typedef enum
{
    R, ///< Reduce operation.
    S, ///< Shift operation.
    E, ///< Equal operation.
    F, ///< Error operation.
    O  ///< OK operation.
} precValues;

/**
 * @brief Enum representing the indices in the precedence table.
 */
typedef enum
{
    I_PLUS, ///< Plus operation.
    I_MULTIPLY, ///< Multiply operation.
    I_DATA, ///< Data.
    I_DOLLAR, ///< End of input.
    I_RELATIONAL, ///< Relational operation.
    I_OPENB, ///< Open bracket.
    I_CLOSEB, ///< Close bracket.
    I_NOTNIl, ///< Not nil operation.
    I_VALORNIL ///< Value or nil operation.
} tableIndex;

/**
 * @brief Function to parse an expression.
 * @param scanner The scanner to get the tokens from.
 * @param endWithBracket Flag indicating if the expression ends with a bracket.
 * @return 0 if successful, non-zero otherwise.
 */
int parseExpression(Scanner *scanner, bool endWithBracket);

#endif //IFJ23_EXPRESSION_H