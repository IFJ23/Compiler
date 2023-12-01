// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Ivan Onufriienko (xonufr00)

#ifndef IFJ23_SYMSTACK_H
#define IFJ23_SYMSTACK_H

#include "scanner.h"
#include "error.h"
typedef struct StackItem
{
    Token t;
    struct StackItem *next;
} StackItem;

typedef struct
{
    StackItem *head;
    unsigned len;
} Stack;

void stackFree(Stack *s);

void stackInit(Stack *s);

void stackPush(Stack *s, Token t);

void stackPop(Stack *s, Token *t);

#endif
