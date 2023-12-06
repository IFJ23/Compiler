// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file generator.h
 * @brief Code generation for the IFJ23 language compiler
 * @author Oleksii Shelest (xshele02)
 */

#ifndef IFJ23_GENERATOR_H
#define IFJ23_GENERATOR_H

#include "stdio.h"
#include "scanner.h"
#include "parser.h"

/**
 * @brief Function to generate the header of the output code.
 */
void genPrintHead();

/**
 * @brief Function to generate code for pushing a token onto the stack.
 * @param t The token to push onto the stack.
 */
void genStackPush(Token t);

/**
 * @brief Function to generate code for creating a new frame.
 */
void genFRAME();

/**
 * @brief Function to generate code for a plus operation.
 */
void genPLUS();

/**
 * @brief Function to generate code for a minus operation.
 */
void genMINUS();

/**
 * @brief Function to generate code for a multiply operation.
 */
void genMULTIPLY();

/**
 * @brief Function to generate code for a division operation.
 */
void genDIVISION();

/**
 * @brief Function to generate code for an equals operation.
 */
void genEQUALS();

/**
 * @brief Function to generate code for a more or less operation.
 */
void genMORE_LESS();

/**
 * @brief Function to generate code for a coalescing operation.
 */
void genCOALESCING();

/**
 * @brief Function to generate code for checking an operator.
 */
void genOperatorCheck();

/**
 * @brief Function to generate code for the beginning of an expression.
 */
void genExpressionBegin();

/**
 * @brief Function to generate code for the end of an expression.
 */
void genExpressionEnd();

/**
 * @brief Function to generate code for checking the truth value of an expression.
 */
void genCheckTruth();

/**
 * @brief Function to generate code for the first part of an if-else statement.
 * @param num The number of the if-else statement.
 */
void genIfElse1(int num);

/**
 * @brief Function to generate code for the second part of an if-else statement.
 * @param num The number of the if-else statement.
 */
void genIfElse2(int num);

/**
 * @brief Function to generate code for the third part of an if-else statement.
 * @param num The number of the if-else statement.
 */
void genIfElse3(int num);

/**
 * @brief Function to generate code for the first part of a while loop.
 * @param num The number of the while loop.
 */
void genWhileLoop1(int num);

/**
 * @brief Function to generate code for the second part of a while loop.
 * @param num The number of the while loop.
 */
void genWhileLoop2(int num);

/**
 * @brief Function to generate code for the third part of a while loop.
 * @param num The number of the while loop.
 */
void genWhileLoop3(int num);

/**
 * @brief Function to generate code for defining a variable.
 * @param t The token representing the variable.
 */
void genDefineVariable(Token t);

/**
 * @brief Function to generate code for assigning a value to a variable.
 * @param t The token representing the variable.
 */
void genAssignVariable(Token t);

/**
 * @brief Function to generate code for checking if a variable is defined.
 * @param t The token representing the variable.
 */
void genCheckDefined(Token t);

/**
 * @brief Function to generate code for the first part of a function definition.
 * @param funcname The name of the function.
 * @param parCount The number of parameters of the function.
 * @param ll The linked list of parameters.
 */
void genFuncDef1(char *funcname, int parCount, LinkedList ll);

/**
 * @brief Function to generate code for the second part of a function definition.
 * @param funcname The name of the function.
 */
void genFuncDef2(char *funcname);

/**
 * @brief Function to generate code for a function call.
 * @param funcname The name of the function.
 * @param paramCount The number of parameters of the function.
 * @param returnType The return type of the function.
 */
void genFuncCall(char *funcname, int paramCount, ListNode *returnType);

/**
 * @brief Function to generate code for a return statement.
 * @param funcname The name of the function.
 * @param expr Flag indicating if there is an expression in the return statement.
 */
void genReturn(char *funcname, bool expr);

/**
 * @brief Function to generate code for converting a value to boolean.
 */
void genConvertBool();

#endif // IFJ23_GENERATOR_H