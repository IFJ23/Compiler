// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Ivan Onufriienko (xonufr00)

#include "symtable.h"
#include "error.h"


size_t hash_function(const char *str) {
    size_t h = 0;
    const unsigned char *p;
    
    for (p = (const unsigned char *) str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}

hash_tab_t *symtable_init(const size_t size) {
    hash_tab_t *table = malloc(sizeof(hash_tab_t));
    
    if (table == NULL) {
        return NULL;
    }
    
    table->size = 0;
    table->arr_size = size;
    table->arr_ptr = malloc(table->arr_size * sizeof(hash_tab_item_t *));    
    
    if (table->arr_ptr == NULL) {
        free(table);
        return NULL;
    }
    
    for (int i = 0; i < table->arr_size; i++) {
        table->arr_ptr[i] = NULL;
    }

    return table;
}