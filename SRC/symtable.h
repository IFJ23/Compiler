// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)

#ifndef IFJ23_SYMTABLE_H
#define IFJ23_SYMTABLE_H

#include <string.h>
#include <stdbool.h>
#include "structures.h"

typedef const char *SymtableKey;

typedef enum
{
    FUNC,
    VAR
} ElType;

typedef struct SymtableData
{
    ElType type;
    int paramsCnt;
    bool possiblyUndefined;
    LinkedList parameters;
} SymtableData;

typedef struct SymtablePair
{
    SymtableKey key;
    SymtableData data;
} SymtablePair;

typedef struct SymtableItem
{
    SymtablePair pair;
    struct SymtableItem *next;
} SymtableItem;

typedef struct Symtable
{
    size_t size;
    size_t arr_size;
    SymtableItem **arr_ptr;
} Symtable;

/**
 * @brief Calculates hash given a key.
 *
 * @param key Key to calculate hash for.
 * @return size_t Hash of supplemented key.
 */
size_t symtableHash(SymtableKey key);

/**
 * @brief Initializes a symtable.
 *
 * @param n Size of pointer array.
 * @return symtable* Pointer to the initialized symtable.
 */
Symtable *symtableInit(size_t n);

/**
 * @brief Searches for a key in a symtable.
 *
 * @param t The symtable to search in.
 * @param key The key to look for.
 * @return SymtablePair* Found item.
 */
SymtablePair *symtableFind(Symtable *t, SymtableKey key);

/**
 * @brief Adds a new element to a symtable.
 *
 * @param t The symtable to add the element to.
 * @param key Key of the element.
 * @param type Type of the element.
 * @param paramsCnt Number of parameters (only for functions).
 * @param undefined Indentifier was defined in an if statement, thus possibly not defined.
 * @param params Linked list of function parameters (only for functions).
 * @return SymtablePair* Pointer to the added element.
 */
SymtablePair *symtableAdd(Symtable *t, SymtableKey key, ElType type, int paramsCnt, bool undefined, LinkedList params);

/**
 * @brief Frees a symtable.
 *
 * @param t The symtable to free.
 */
void symtableFree(Symtable *t);

#endif
