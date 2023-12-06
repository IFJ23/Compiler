// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Ivan Onufriienko (xonufr00)

#include "linlist.h"

// Function to initialize a linked list
void listInit(LinkedList *l)
{
    l->head = NULL;       // Set the head of the linked list to NULL, indicating an empty list
    l->active = NULL;     // Set the active node to NULL
    l->itemCount = 0;     // Initialize the count of items to 0
}

// Function to insert a new node into the linked list
int listInsert(LinkedList *l, Keyword type, char *name)
{
    ListNode *newEle = malloc(sizeof(struct ListNode));  // Allocate memory for a new list node
    if (newEle == NULL)
        return INTERNAL_ERROR;  // Return an error code if memory allocation fails
    else
    {
        newEle->type = type;     // Set the type of the new node
        newEle->name = name;     // Set the name of the new node
        newEle->next = NULL;     // Initialize the next pointer to NULL

        if (l->head == NULL) {
            l->head = newEle;    // If the list is empty, set the new node as the head
            l->active = newEle;  // Set the active node to the new node
        }
        else {
            l->active->next = newEle;  // Append the new node to the end of the list
            l->active = l->active->next;  // Set the active node to the newly added node
        }
        ++(l->itemCount);           // Increment the count of items in the list
    }

    return 0;  // Return 0, indicating success
}

// Function to dispose of the linked list and free associated memory
void listDispose(LinkedList *l)
{
    ListNode *nextE;
    while (l->head != NULL)
    {
        nextE = l->head->next;  // Save the next node
        free(l->head);          // Free the current node
        l->head = nextE;        // Move to the next node
    }
    l->active = NULL;          // Set the active node to NULL
    l->itemCount = 0;          // Reset the count of items to 0
}

// Function to get a node by its index in the linked list
ListNode *listGetByIndex(LinkedList *l, int index)
{
    if (index < 0 || index >= l->itemCount) {
        return NULL; // Return NULL if the index is out of bounds
    }

    ListNode *current = l->head;  // Start at the head of the linked list
    int currentIndex = 0;

    while (current != NULL && currentIndex < index) {
        current = current->next;   // Traverse the list until the desired index is reached
        currentIndex++;
    }

    return current;  // Return the node at the specified index
}