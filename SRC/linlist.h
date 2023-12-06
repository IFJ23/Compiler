// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file linlist.h
 * @brief Linked list for the IFJ23 language compiler
 * @author Vsevolod Pokhvalenko (xpokhv00)
 */
#ifndef IFJ23_LINLIST_H
#define IFJ23_LINLIST_H

#include "scanner.h"
#include "error.h"

/**
 * @brief Struct representing a node in the linked list.
 */
typedef struct ListNode
{
    Keyword type; ///< The type of the element stored in the node.
    bool opt; ///< Flag indicating if the element is optional.
    char *name; ///< The name of the element.
    char *id; ///< The identifier of the element.
    struct ListNode *next; ///< Pointer to the next node in the list.
} ListNode;

/**
 * @brief Struct representing a linked list.
 */
typedef struct
{
    struct ListNode *head; ///< Pointer to the head of the list.
    struct ListNode *active; ///< Pointer to the active node in the list.
    int itemCount; ///< The current count of items in the list.
} LinkedList;

/**
 * @brief Function to initialize a linked list.
 * @param l Pointer to the list to be initialized.
 */
void listInit(LinkedList *l);

/**
 * @brief Function to insert an element into the list.
 * @param l Pointer to the list.
 * @param type The type of the element to insert.
 * @param name The name of the element to insert.
 * @return 0 if successful, non-zero otherwise.
 */
int listInsert(LinkedList *l, Keyword type, char *name);

/**
 * @brief Function to get a node by its index in the list.
 * @param l Pointer to the list.
 * @param index The index of the node to get.
 * @return Pointer to the node, or NULL if not found.
 */
ListNode *listGetByIndex(LinkedList *l, int index);

/**
 * @brief Function to free the memory allocated for the list.
 * @param l Pointer to the list to be freed.
 */
void listDispose(LinkedList *l);

#endif
