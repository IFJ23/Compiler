// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Ivan Onufriienko (xonufr00)

#include "symstack.h"

// Function to initialize the stack
void stackInit(Stack *s)
{
    s->length = 0;      // Initialize the length of the stack to 0
    s->top = NULL;  // Set the head of the stack to NULL, indicating an empty stack
}

// Function to push a token onto the stack
void stackPush(Stack *s, Token t)
{
    if (s == NULL)    // Check if the stack is NULL
        return;

    // Allocate memory for a new stack item
    StackItem *newItem = (StackItem *)malloc(sizeof(StackItem));
    if (newItem == NULL)  // Check if memory allocation failed
        return;

    newItem->t = t;             // Set the token in the new stack item
    newItem->next = s->top;    // Set the next pointer of the new item to the current top of the stack
    s->top = newItem;          // Update the head of the stack to the new item
    ++(s->length);                 // Increment the length of the stack
}

// Function to pop a token from the stack
void stackPop(Stack *s, Token *t)
{
    if (s == NULL || s->top == NULL)  // Check if the stack or the head of the stack is NULL
        return;

    // Temporary variable to hold the current top of the stack
    StackItem *tmp = s->top;
    if (t != NULL)
        *t = tmp->t;            // If the 't' pointer is not NULL, store the token in the provided location
    s->top = s->top->next;    // Update the head of the stack to the next item
    free(tmp);                  // Free the memory of the popped item
    --(s->length);                 // Decrement the length of the stack
}

// Function to free the memory used by the stack
void stackFree(Stack *s)
{
    if (s == NULL)   // Check if the stack is NULL
        return;

    // Temporary variable to traverse the stack
    StackItem *tmp = s->top;
    while (tmp != NULL)
    {
        tmp = s->top->next;   // Move to the next item in the stack
        free(s->top);         // Free the memory of the current item
        s->top = tmp;         // Update the head of the stack
    }

    s->length = 0;  // Set the length of the stack to 0
}
