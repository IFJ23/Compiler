// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Oleksii Shelest (xshele02)

#ifndef IFJ23_GENERATOR_H
#define IFJ23_GENERATOR_H

#include "scanner.h"
#include "parser.h"

/**
 * @brief Function to generate the header of the output code.
 */
void generatorHead();

/**
 * @brief Function to generate code for pushing a token onto the stack.
 * @param t The token to push onto the stack.
 */
void generatorStackPush(Token t);

/**
 * @brief Function to generate code for creating a new frame.
 */
void generatorFRAME();

/**
 * @brief Function to generate code for a plus operation.
 */
void generatorPLUS();

/**
 * @brief Function to generate code for a minus operation.
 */
void generatorMINUS();

/**
 * @brief Function to generate code for a multiply operation.
 */
void generatorMULTIPLY();

/**
 * @brief Function to generate code for a division operation.
 */
void generatorDIVISION();

/**
 * @brief Function to generate code for an equals operation.
 */
void generatorEQUALS();

/**
 * @brief Function to generate code for a more or less operation.
 */
void generatorMORE_LESS();

/**
 * @brief Function to generate code for a coalescing operation.
 */
void generatorCOALESCING();

/**
 * @brief Function to generate code for the beginning of an expression.
 */
void generatorExpressionBegin();

/**
 * @brief Function to generate code for the end of an expression.
 */
void generatorExpressionEnd();

/**
 * @brief Function to generate code for the first part of a while loop.
 * @param num The number of the while loop.
 */
void generatorIfElse1(int num);

/**
 * @brief Function to generate code for the second part of a while loop.
 * @param num The number of the while loop.
 */
void generatorIfElse2(int num);

/**
 * @brief Function to generate code for the third part of a while loop.
 * @param num The number of the while loop.
 */
void generatorIfElse3(int num);

/**
 * @brief Function to generate code for the first part of a while loop.
 * @param num The number of the while loop.
 */
void generatorWhile1(int num);

/**
 * @brief Function to generate code for the second part of a while loop.
 * @param num The number of the while loop.
 */
void generatorWhile2(int num);

/**
 * @brief Function to generate code for the third part of a while loop.
 * @param num The number of the while loop.
 */
void generatorWhile3(int num);

/**
 * @brief Function to generate code for defining a variable.
 * @param t The token representing the variable.
 */
void generatorDefineVariable(Token t);

/**
 * @brief Function to generate code for assigning a value to a variable.
 * @param t The token representing the variable.
 */
void generatorAssignVariable(Token t);

/**
 * @brief Function to generate code for checking if a variable is defined.
 * @param t The token representing the variable.
 */
void generatorCheckDefined(Token t);

/**
 * @brief Function to generate code for a function call.
 * @param funcname The name of the function.
 * @param paramCount The number of parameters of the function.
 * @param returnType The return type of the function.
 */
void generatorFuncCall(char *funcname, int paramCount, ListNode *returnType);

/**
 * @brief Function to generate code for a return statement.
 * @param funcname The name of the function.
 * @param expr Flag indicating if there is an expression in the return statement.
 */
void generatorReturn(char *funcname, bool expr);

/**
 * @brief Function to generate code for converting a value to boolean.
 */
void generatorConvert_TRUE_FALSE();

#endif // IFJ23_GENERATOR_H