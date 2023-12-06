// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Ivan Onufriienko (xonufr00)

#include <stdlib.h>
#include <stdint.h>
#include "symtable.h"

// Function to calculate the hash value for a given key
size_t symtableHash(const char *str)
{
    uint32_t h = 0;                         // Initialize the hash value to 0
    const unsigned char *p;                // Pointer to traverse the string
    for (p = (const unsigned char *)str; *p != '\0'; p++)
        h = 65599 * h + *p;                // Hashing algorithm (a prime number used as a multiplier)
    return h;                              // Return the calculated hash value
}

// Function to initialize a symbol table with a given size
Symtable *symtableInit(size_t n)
{
    Symtable *table = (Symtable *)malloc(sizeof(Symtable));  // Allocate memory for the symbol table
    if (!table)
        return NULL;

    table->size = 0;                                            // Initialize the number of elements to 0
    table->array_size = n;                                        // Set the size of the array of linked lists
    table->array_ptr = malloc(sizeof(SymtableItem *) * n);   // Allocate memory for the array of linked lists
    if (!table->array_ptr)
    {
        free(table);                // Free the previously allocated memory if the allocation fails
        return NULL;
    }

    for (size_t i = 0; i < n; i++)
    {
        table->array_ptr[i] = NULL;   // Initialize each linked list in the array to NULL
    }

    return table;                   // Return the initialized symbol table
}

// Function to add a key-value pair to the symbol table
SymtablePair *symtableAdd(Symtable *table, SymtableKey key, ElemType type, int paramsCnt, bool undefined, LinkedList params)
{
    if (!table)
        return NULL;

    SymtablePair *found = symtableFind(table, key);       // Check if the key already exists in the symbol table

    if (found)
        return found;                                // Return the existing pair if the key is found

    SymtableItem *newitem = (SymtableItem *)malloc(sizeof(SymtableItem));  // Allocate memory for a new item in the linked list
    if (!newitem)
    {
        return NULL;                                    // Return NULL if memory allocation fails
    }

    size_t len = strlen(key);                        // Calculate the length of the key
    newitem->pair.key = calloc(len + 1, 1);  // Allocate memory for the key and initialize to 0
    if (!newitem->pair.key)
    {
        free(newitem);                                  // Free the allocated memory if allocation fails
        return NULL;
    }

    memcpy((char *)newitem->pair.key, key, len);        // Copy the key to the allocated memory
    newitem->pair.value.type = type;                     // Set the type in the value structure
    newitem->pair.value.undefined = undefined;   // Set the possiblyUndefined flag in the value structure
    newitem->pair.value.parameters = params;             // Set the parameters in the value structure
    newitem->pair.value.paramsCnt = paramsCnt;           // Set the number of parameters in the value structure
    newitem->next = NULL;                               // Initialize the next pointer to NULL
    table->size+=1;                                          // Increment the number of elements in the symbol table

    size_t index = symtableHash(key) % table->array_size; // Calculate the index in the array of linked lists
    SymtableItem *tmp = table->array_ptr[index];

    if (!tmp)
        table->array_ptr[index] = newitem;                // If the linked list is empty, set the new item as the head
    else
    {
        while (tmp->next)
            tmp = tmp->next;                        // Traverse to the end of the linked list
        tmp->next = newitem;                        // Set the new item as the last element in the linked list
    }

    return &(newitem->pair);                        // Return the added pair
}


// Function to find a key in the symbol table and return its associated data
SymtablePair *symtableFind(Symtable *table, SymtableKey key)
{
    if (!table)
        return NULL;

    size_t hash = symtableHash(key) % table->array_size;   // Calculate the hash value for the key
    SymtableItem *currentItem = table->array_ptr[hash];            // Get the linked list at the calculated index

    while (currentItem)
    {
        if (!strcmp(key, currentItem->pair.key))
        {
            return &(currentItem->pair);    // Return the pair if the key is found
        }
        currentItem = currentItem->next;            // Move to the next item in the linked list
    }
    return NULL;                    // Return NULL if the key is not found
}

// Function to free the memory used by the symbol table
void symtableFree(Symtable *table)
{
    if (!table)
        return;

    SymtableItem *currItem, *next;

    for (size_t i = 0; i < table->array_size; i++)
    {
        currItem = table->array_ptr[i];
        while (currItem)
        {
            next = currItem->next;
            free((char *)currItem->pair.key);          // Free the memory used by the key
            free(currItem);                            // Free the memory used by the current item
            currItem = next;                           // Move to the next item in the linked list
        }
        table->array_ptr[i] = NULL;                      // Set the linked list to NULL after freeing all items
    }
    table->size = 0;                                   // Reset the number of elements to 0
    free(table->array_ptr);                              // Free the memory used by the array of linked lists
    free(table);                                       // Free the memory used by the symbol table
}