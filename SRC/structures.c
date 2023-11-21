// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)

#include "structures.h"

void stackInit(Stack *s)
{
    s->head = NULL;
    s->len = 0;
}

void stackPush(Stack *s, Token t)
{
    if (s == NULL)
        return;

    StackItem *newItem = malloc(sizeof(StackItem));
    if (newItem == NULL)
        return;

    newItem->t = t;
    newItem->next = s->head;
    s->head = newItem;
    ++(s->len);
}

void stackPop(Stack *s, Token *t)
{
    if (s == NULL || s->head == NULL)
        return;

    StackItem *tmp = s->head;
    if (t != NULL)
        *t = tmp->t;
    s->head = s->head->next;
    free(tmp);
    --(s->len);
}

void stackFree(Stack *s)
{
    if (s == NULL)
        return;

    StackItem *tmp = s->head;
    while (tmp != NULL)
    {
        tmp = s->head->next;
        free(s->head);
        s->head = tmp;
    }

    s->len = 0;
}

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
