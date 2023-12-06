// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file linlist.c
 * @brief Linked list for the IFJ23 language compiler
 * @author Vsevolod Pokhvalenko (xpokhv00)
 */
#include "linlist.h"

void listInit(LinkedList *l)
{
    l->head = NULL;
    l->itemCount = 0;
}

int listInsert(LinkedList *l, Keyword type, char *name)
{
    ListNode *newEle = malloc(sizeof(struct ListNode));
    if (newEle == NULL)
        return INTERNAL_ERROR;
    else
    {
        newEle->type = type;
        newEle->name = name;
        newEle->next = NULL;

        if (l->head == NULL) {
            l->head = newEle;
            l->active = newEle;
        }
        else {
            l->active->next = newEle;
            l->active = l->active->next;
        }
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
    l->active = NULL;
    l->itemCount = 0;
}

ListNode *listGetByIndex(LinkedList *l, int index)
{
    if (index < 0 || index >= l->itemCount) {
        return NULL; // Index out of bounds
    }

    ListNode *current = l->head;
    int currentIndex = 0;

    while (current != NULL && currentIndex < index) {
        current = current->next;
        currentIndex++;
    }

    return current;
}
