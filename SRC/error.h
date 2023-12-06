// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file error.h
 * @brief Error codes and handling for the IFJ23 language compiler
 * @author Ivan Onufriienko (xonufr00)
 */

#ifndef IFJ23_ERROR_H
#define IFJ23_ERROR_H

/**
 * @brief Error code for incorrect structure of the current lexeme.
 */
#define LEXICAL_ERROR 1

/**
 * @brief Error code for incorrect syntax of the program, missing header, etc.
 */
#define SYNTAX_ERROR 2

/**
 * @brief Error code for undefined function, variable redefinition.
 */
#define SEMANTIC_DEFINITION_ERROR 3

/**
 * @brief Error code for wrong number/type of parameters when calling the function or wrong type of return value from function.
 */
#define SEMANTIC_TYPE_ERROR 4

/**
 * @brief Error code for use of an undefined or uninitialized variable.
 */
#define SEMANTIC_UNDEFINED_ERROR 5

/**
 * @brief Error code for missing/remaining expression in the return command from the function.
 */
#define SEMANTIC_RETURN_ERROR 6

/**
 * @brief Error code for compatibility error in arithmetic, string and relational expressions.
 */
#define SEMANTIC_COMPATIBILITY_ERROR 7

/**
 * @brief Error code for inference error - variable or parameter type is not specified and cannot be inferred from the expression used.
 */
#define SEMANTIC_UNDECLARED_ERROR 8

/**
 * @brief Error code for other semantic errors.
 */
#define SEMANTIC_OTHER_ERROR 9

/**
 * @brief Error code for internal compiler error.
 */
#define INTERNAL_ERROR 99

/**
 * @brief Function to print an error message.
 * @param line The line number where the error occurred.
 * @param errorText The error message to print.
 */
void printError(int line, char *errorText);

#endif //IFJ23_ERROR_H