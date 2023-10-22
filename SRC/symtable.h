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

typedef char * hash_tab_key_t; 

typedef enum{
    HASH_TYPE_FUNCTION,
    HASH_TYPE_VARIABLE,
    HASH_TYPE_CONSTANT,
} hash_tab_type_t;


typedef enum {
    DATA_TYPE_INT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_STRING,
    DATA_TYPE_UNDEFINED,
    DATA_TYPE_VOID,
} hash_tab_data_t;

typedef struct hashtab_data {
    hash_tab_type_t type;
    hash_tab_data_t value_type;
    hash_tab_data_t return_type;
    hash_tab_data_t* params;
    int params_count;
    char** params_names;
} hash_tab_data_t;       

typedef struct hash_tab_item{
    hash_tab_data_t data;
    struct hash_tab_item* next;
} hash_tab_item_t;

typedef struct hash_tab{
    size_t size;
    size_t arr_size;
    hash_tab_item_t** arr_ptr;
} hash_tab_t;

size_t hash_function(hash_tab_key_t key, size_t size);

hash_tab_t* hash_tab_init(size_t size);

hash_tab_item_t* hash_tab_find(hash_tab_t* table, hash_tab_key_t key);

hash_tab_item_t* hash_tab_insert(hash_tab_t* table, hash_tab_key_t key, hash_tab_data_t data);

void hash_tab_for_each(hash_tab_t* table, void (*func)(hash_tab_item_t* item));

void hash_tab_clear(hash_tab_t* table);

void hash_tab_free(hash_tab_t* table);



#endif // HTAB_H__