// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Ivan Onufriienko (xonufr00)

#ifndef IFJ23_STRUCTURES_H
#define IFJ23_STRUCTURES_H

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

typedef struct ListNode
{
    Keyword type;
    bool opt;
    char *name;
    char *id;
    struct ListNode *next;
} ListNode;

typedef struct
{
    struct ListNode *head;
    struct ListNode *last;
    int itemCount;
} LinkedList;


void stackFree(Stack *s);

void listInit(LinkedList *l);

int listInsert(LinkedList *l, Keyword type);

void listDispose(LinkedList *l);

void stackInit(Stack *s);

void stackPush(Stack *s, Token t);

void stackPop(Stack *s, Token *t);

#endif
