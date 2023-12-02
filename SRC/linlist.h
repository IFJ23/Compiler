// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Ivan Onufriienko (xonufr00)

#ifndef IFJ23_LINLIST_H
#define IFJ23_LINLIST_H

#include "scanner.h"
#include "error.h"

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
    struct ListNode *active;
    int itemCount;
} LinkedList;

void listInit(LinkedList *l);

int listInsert(LinkedList *l, Keyword type, char *name);

ListNode *listGetByIndex(LinkedList *l, int index);

void listDispose(LinkedList *l);

#endif
