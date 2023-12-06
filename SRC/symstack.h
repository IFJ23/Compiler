// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file symstack.h
 * @brief Stack for symbols in the IFJ23 language compiler
 * @author Vsevolod Pokhvalenko (xpokhv00)
 */
#ifndef IFJ23_SYMSTACK_H
#define IFJ23_SYMSTACK_H

#include "scanner.h"
#include "error.h"

/**
 * @brief Struct representing an item in the stack.
 * @param t The token stored in the stack item.
 * @param next Pointer to the next item in the stack.
 */
typedef struct StackItem
{
    Token t;
    struct StackItem *next;
} StackItem;

/**
 * @brief Struct representing a stack.
 * @param head Pointer to the first item in the stack.
 * @param len Length of the stack.
 */
typedef struct
{
    StackItem *head;
    unsigned len;
} Stack;

/**
 * @brief Function to free the memory allocated for the stack.
 * @param s Pointer to the stack to be freed.
 */
void stackFree(Stack *s);

/**
 * @brief Function to initialize the stack.
 * @param s Pointer to the stack to be initialized.
 */
void stackInit(Stack *s);

/**
 * @brief Function to push a token onto the stack.
 * @param s Pointer to the stack.
 * @param t The token to be pushed onto the stack.
 */
void stackPush(Stack *s, Token t);

/**
 * @brief Function to pop a token from the stack.
 * @param s Pointer to the stack.
 * @param t Pointer to the token where the popped token will be stored.
 */
void stackPop(Stack *s, Token *t);

#endif
