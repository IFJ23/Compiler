// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file symtable.h
 * @brief Stack for symbols in the IFJ23 language compiler
 * @author Vsevolod Pokhvalenko (xpokhv00)
 * @author Sviatoslav Pokhvalenko (xpokhv01)
 */

#ifndef IFJ23_SYMTABLE_H
#define IFJ23_SYMTABLE_H

#include <string.h>
#include <stdbool.h>
#include "linlist.h"
#include "symstack.h"

typedef const char *SymtableKey;

/**
 * @brief Enum representing the type of an element in the symbol table.
 */
typedef enum
{
    VAR,
    LET,
    FUNC
} ElemType;

/**
 * @brief Struct representing the data of an element in the symbol table.
 * @param type The type of the element.
 * @param paramsCnt The number of parameters of the element.
 * @param possiblyUndefined Whether the element can be undefined.
 * @param parameters The list of parameters of the element.
 */
typedef struct SymtableData
{
    ElemType type;
    int paramsCnt;
    bool possiblyUndefined;
    LinkedList parameters;
} SymtableValue;

/**
 * @brief Struct representing a key-value pair in the symbol table.
 * @param key The key of the pair.
 * @param data The data of the pair.
 */
typedef struct SymtablePair
{
    SymtableKey key;
    SymtableValue value;
} SymtablePair;

/**
 * @brief Struct representing an item in the symbol table.
 * @param pair The key-value pair of the item.
 * @param next Pointer to the next item in the symbol table.
 */
typedef struct SymtableItem
{
    SymtablePair pair;
    struct SymtableItem *next;
} SymtableItem;

/**
 * @brief Struct representing a symbol table.
 * @param size The number of elements in the symbol table.
 * @param arr_size The size of the array of pointers to items in the symbol table.
 * @param arr_ptr The array of pointers to items in the symbol table.
 */
typedef struct Symtable
{
    size_t size;
    size_t arr_size;
    SymtableItem **arr_ptr;
} Symtable;

/**
 * @brief Function to compute the hash of a key.
 * @param key The key to compute the hash for.
 * @return The computed hash.
 */
size_t symtableHash(SymtableKey key);

/**
 * @brief Function to initialize a symbol table.
 * @param n The initial size of the symbol table.
 * @return Pointer to the initialized symbol table.
 */
Symtable *symtableInit(size_t n);

/**
 * @brief Function to find a key-value pair in the symbol table.
 * @param t Pointer to the symbol table.
 * @param key The key to find.
 * @return Pointer to the found key-value pair, or NULL if not found.
 */
SymtablePair *symtableFind(Symtable *t, SymtableKey key);

/**
 * @brief Function to add a key-value pair to the symbol table.
 * @param t Pointer to the symbol table.
 * @param key The key of the pair to add.
 * @param type The type of the element to add.
 * @param paramsCnt The count of parameters if the element is a function.
 * @param undefined Flag indicating if the element is possibly undefined.
 * @param params List of parameters if the element is a function.
 * @return Pointer to the added key-value pair.
 */
SymtablePair *symtableAdd(Symtable *t, SymtableKey key, ElemType type, int paramsCnt, bool undefined, LinkedList params);


/**
 * @brief Function to free the memory allocated for the symbol table.
 * @param t Pointer to the symbol table to be freed.
 */
void symtableFree(Symtable *t);

#endif
