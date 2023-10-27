// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)

#include "stack.h"
#include <stdlib.h>

int STACK_SIZE = MAX_STACK;
bool error_flag;
bool solved;

void Stack_Error( int error_code ) {
    static const char *SERR_STRINGS[MAX_SERR + 1] = {
            "Unknown error",
            "Stack error: INIT",
            "Stack error: PUSH",
            "Stack error: TOP"
    };

    if (error_code <= 0 || error_code > MAX_SERR)
    {
        error_code = 0;
    }

    printf("%s\n", SERR_STRINGS[error_code]);
    error_flag = true;
}

void Stack_Init( Stack *stack ) {
    // Check if the provided stack pointer is NULL
    if (stack == NULL) {
        // If it's NULL, report an initialization error
        Stack_Error(SERR_INIT);
        return;
    }

    // Allocate memory for the dynamic array within the stack
    stack->array = (char *)malloc(STACK_SIZE * sizeof(char));

    // Check if memory allocation was successful
    if (stack->array == NULL) {
        // If not, report an initialization error
        Stack_Error(SERR_INIT);
        return;
    }

    // Initialize the 'topIndex' to -1, indicating an empty stack
    stack->topIndex = -1; // Initialize the stack as empty
}

bool Stack_IsEmpty( const Stack *stack ) {
    // Check if the 'topIndex' of the stack is -1, indicating an empty stack
    return stack->topIndex == -1;
}

bool Stack_IsFull( const Stack *stack ) {
    return stack->topIndex == STACK_SIZE - 1;
}

void Stack_Top( const Stack *stack, char *dataPtr ) {
    // Check if the stack is empty; if so, report a top operation error
    if (Stack_IsEmpty(stack)) {
        Stack_Error(SERR_TOP);
        return;
    }

    // Copy the value from the top of the stack into the 'dataPtr'
    *dataPtr = stack->array[stack->topIndex];
}

void Stack_Pop( Stack *stack ) {
    // Check if the stack is not empty before popping an element
    if (!Stack_IsEmpty(stack)) {
        // Decrement the 'topIndex' to effectively remove the element from the top
        stack->topIndex--;
    }
}

void Stack_Push( Stack *stack, char data ) {
    // Check if the stack is full before pushing a new element
    if (Stack_IsFull(stack)) {
        // If the stack is full, report a push operation error
        Stack_Error(SERR_PUSH);
        return;
    }

    // Increment the 'topIndex' to point to the next available position
    stack->topIndex++;

    // Store the 'data' in the array at the updated 'topIndex'
    stack->array[stack->topIndex] = data;
}

void Stack_Dispose( Stack *stack ) {
    // Deallocate the memory used for the array
    free(stack->array);

    // Set the 'array' pointer to NULL to avoid potential issues
    stack->array = NULL;

    // Reset the 'topIndex' to -1, indicating an empty stack
    stack->topIndex = -1;
}