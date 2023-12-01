// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Ivan Onufriienko (xonufr00)

#include "linlist.h"

void listInit(LinkedList *l)
{
    l->head = NULL;
    l->itemCount = 0;
}

int listInsert(LinkedList *l, Keyword type)
{
    ListNode *newEle = malloc(sizeof(struct ListNode));
    if (newEle == NULL)
        return INTERNAL_ERROR;
    else
    {
        newEle->type = type;
        newEle->next = NULL;
        if (l->head == NULL)
            l->head = newEle;
        else
            l->last->next = newEle;
        l->last = newEle;
        ++(l->itemCount);
    }

    return 0;
}

void listDispose(LinkedList *l)
{
    ListNode *nextE;
    while (l->head != NULL)
    {
        nextE = l->head->next;
        free(l->head);
        l->head = nextE;
    }
    l->last = NULL;
    l->itemCount = 0;
}
