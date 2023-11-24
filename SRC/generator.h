// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Oleksii Shelest (xshele02)


#ifndef H_GENERATOR
#define H_GENERATOR

#include "scanner.h"
#include "parser.h"


void genPrintHead();


void genStackPush(Token t);


void genMathInstCheck();


void genExpressionBegin();


void genExpressionEnd();

void genCheckTruth();

void genIfElse1(int num);

void genIfElse2(int num);

void genIfElse3(int num);

void genWhileLoop1(int num);

void genWhileLoop2(int num);

void genWhileLoop3(int num);

void genDefineVariable(Token t);

void genAssignVariable(Token t);

void genCheckDefined(Token t);

void genFuncDef1(char *funcname, int parCount, LinkedList ll);

void genFuncDef2(char *funcname);

void genFuncCall(char *funcname, int paramCount, ListNode *returnType);

void genReturn(char *funcname, bool expr);

void genPushString(char *str);

#endif
