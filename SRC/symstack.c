// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Ivan Onufriienko (xonufr00)

#include "symstack.h"

// Function to initialize the stack
void stackInit(Stack *s)
{
    s->head = NULL;  // Set the head of the stack to NULL, indicating an empty stack
    s->len = 0;      // Initialize the length of the stack to 0
}

// Function to push a token onto the stack
void stackPush(Stack *s, Token t)
{
    if (s == NULL)    // Check if the stack is NULL
        return;

    // Allocate memory for a new stack item
    StackItem *newItem = malloc(sizeof(StackItem));
    if (newItem == NULL)  // Check if memory allocation failed
        return;

    newItem->t = t;             // Set the token in the new stack item
    newItem->next = s->head;    // Set the next pointer of the new item to the current top of the stack
    s->head = newItem;          // Update the head of the stack to the new item
    ++(s->len);                 // Increment the length of the stack
}

// Function to pop a token from the stack
void stackPop(Stack *s, Token *t)
{
    if (s == NULL || s->head == NULL)  // Check if the stack or the head of the stack is NULL
        return;

    // Temporary variable to hold the current top of the stack
    StackItem *tmp = s->head;
    if (t != NULL)
        *t = tmp->t;            // If the 't' pointer is not NULL, store the token in the provided location
    s->head = s->head->next;    // Update the head of the stack to the next item
    free(tmp);                  // Free the memory of the popped item
    --(s->len);                 // Decrement the length of the stack
}

// Function to free the memory used by the stack
void stackFree(Stack *s)
{
    if (s == NULL)   // Check if the stack is NULL
        return;

    // Temporary variable to traverse the stack
    StackItem *tmp = s->head;
    while (tmp != NULL)
    {
        tmp = s->head->next;   // Move to the next item in the stack
        free(s->head);         // Free the memory of the current item
        s->head = tmp;         // Update the head of the stack
    }

    s->len = 0;  // Set the length of the stack to 0
}
