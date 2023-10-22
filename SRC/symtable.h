// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Ivan Onufriienko (xonufr00)
#ifndef HTAB_H__
#define HTAB_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "scanner.h"

typedef char * ht_key_t; 

typedef enum{
    ITEM_TYPE_FUNCTION,
    ITEM_TYPE_VARIABLE,
    ITEM_TYPE_CONSTANT,
} ht_item_type_t;


typedef enum {
    DATA_TYPE_INT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_STRING,
    DATA_TYPE_UNDEFINED,
    DATA_TYPE_VOID,
} ht_data_type_t;

typedef struct ht_data {
    ht_item_type_t type;
    ht_data_type_t value_type;
    ht_data_type_t return_type;
    ht_data_type_t* params;
    ht_key_t key;
    int params_count;
    char** params_names;
} ht_data_t;       

typedef struct ht_item{
    ht_data_t data;
    struct ht_item* next;
} ht_item_t;

typedef struct ht_tab{
    size_t size;
    size_t arr_size;
    ht_item_t** arr_ptr;
} ht_t;

size_t hash_function(ht_key_t key, size_t size);

ht_t* ht_init(size_t size);

ht_data_t* ht_find(ht_t* table, ht_key_t key);

ht_data_t* ht_insert(ht_t* table, char* key, Token* token);

void ht_for_each(ht_t* table, void (*func)(ht_data_t* item));

void ht_clear(ht_t* table);

void ht_free(ht_t* table);

#endif // HTAB_H__