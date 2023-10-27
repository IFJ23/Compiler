// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Ivan Onufriienko (xonufr00)

#include "symtable.h"
#include "errors.h"


size_t hash_function(const char *str) {
    size_t h = 0;
    const unsigned char *p;
    
    for (p = (const unsigned char *) str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}

ht_t *ht_init(const size_t size) {
    ht_t *table = malloc(sizeof(ht_t));
    
    if (table == NULL) {
        exit(INTERNAL_ERROR);
    }
    
    table->size = 0;
    table->arr_size = size;
    table->arr_ptr = malloc(table->arr_size * sizeof(ht_item_t *));    
    
    if (table->arr_ptr == NULL) {
        free(table);
        exit(INTERNAL_ERROR);
    }
    
    for (int i = 0; i < table->arr_size; i++) {
        table->arr_ptr[i] = NULL;
    }

    return table;
}

ht_data_t* ht_find(ht_t* table, ht_key_t key) {
    size_t index = hash_function(key) % table->arr_size;
    ht_item_t* item = table->arr_ptr[index];
    
    while (item != NULL) {
        if (strcmp(item->data.key, key) == 0) {
            return &item->data;
        }
        
        item = item->next;
    }
    
    return NULL;
}

ht_data_t* ht_insert(ht_t* table, char* key, Token* token){
        
    ht_data_t* data = malloc(sizeof(ht_data_t));
    
    if(data == NULL){
        exit(INTERNAL_ERROR);
    }
    
    data->key = key;
    data->type = ITEM_TYPE_VARIABLE;
    data->value_type = token->type;
    data->return_type = DATA_TYPE_UNDEFINED;
    data->params = NULL;
    data->params_count = 0;
    data->params_names = NULL;
    
    ht_item_t* item = malloc(sizeof(ht_item_t));
    
    if(item == NULL){
        free(data);
        exit(INTERNAL_ERROR);
    }
    
    item->data = *data;
    item->next = NULL;
    
    size_t index = hash_function(key) % table->arr_size;
    
    if(table->arr_ptr[index] == NULL){
        table->arr_ptr[index] = item;
    }else{
        ht_item_t* tmp = table->arr_ptr[index];
        
        while(tmp->next != NULL){
            tmp = tmp->next;
        }
        
        tmp->next = item;
    }
    
    table->size++;
    
    return data;
}

void ht_for_each(ht_t* table, void (*func)(ht_data_t* item)){
        
        for(int i = 0; i < table->arr_size; i++){
            ht_item_t* item = table->arr_ptr[i];
            
            while(item != NULL){
                func(&item->data);
                item = item->next;
            }
        }
}

void ht_clear(ht_t* table){

    for(int i = 0; i < table->arr_size; i++){
        ht_item_t* item = table->arr_ptr[i];
        
        while(item != NULL){
            ht_item_t* next = item->next;
            free(item->data.key);
            free(item);
            item = next;
        }
    }
    
    table->size = 0;
}

void ht_free(ht_t* table){
    
    if(table == NULL){
        return;
    }
    
    ht_clear(table);
    free(table->arr_ptr);
    free(table);
}
