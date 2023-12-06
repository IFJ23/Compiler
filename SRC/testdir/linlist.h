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
    struct ListNode *next; ///< Pointer to the next node in the list
} ListNode;

/**
 * @brief Struct representing a linked list.
 */
typedef struct
{
    struct ListNode *head;
    struct ListNode *active;
    int itemCount;
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
 * @brief Retrieves a node from the linked list by its index.
 *
 * This function retrieves the node at the specified index position in the linked list.
 * The indexing starts at 0, where 0 represents the first node.
 *
 * @param l Pointer to the linked list.
 * @param index Index of the node to retrieve.
 * @return A pointer to the ListNode at the specified index,
 *         or NULL if the index is out of bounds or the list is empty.
 */
ListNode *listGetByIndex(LinkedList *l, int index);

/**
 * @brief Function to free the memory allocated for the list.
 * @param l Pointer to the list to be freed.
 */
void listDispose(LinkedList *l);

#endif
