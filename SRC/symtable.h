// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)
// Ivan Onufriienko (xonufr00)

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

size_t symtableHash(SymtableKey key);

Symtable *symtableInit(size_t n);

SymtablePair *symtableFind(Symtable *t, SymtableKey key);

SymtablePair *symtableAdd(Symtable *t, SymtableKey key, ElType type, int paramsCnt, bool undefined, LinkedList params);

void symtableFree(Symtable *t);

#endif
