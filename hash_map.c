#include "./hash_map.h"
#include <stdlib.h>
#include <string.h>
#define HASH_MAP_SIZE 256
#define INDEX_MAP_VALUE(MAP, ARR_IND, IND) ((MAP)->arrays[(ARR_IND)].arr + ((IND)*((MAP)->key_size + (MAP)->value_size) + (MAP)->key_size))
#define INDEX_MAP_KEY(MAP, ARR_IND, IND) ((MAP)->arrays[(ARR_IND)].arr + (IND)*((MAP)->key_size + (MAP)->value_size))

typedef struct Array{
    void* arr;
    size_t alloc;
    size_t size;
}Array;

struct HashMap{
    size_t value_size;
    size_t key_size;
    size_t read_value_size;
    size_t read_key_size;
    size_t size;
    size_t (*hash)(const void*);
    int (*equals)(const void*, const void*);
    Array arrays[HASH_MAP_SIZE];
};

struct HashMapIterator{
    HashMap* hash_map;
    size_t index;
    size_t pos;
};

inline size_t hash(const HashMap* self, const void* src) {
    return self->hash(src)%HASH_MAP_SIZE;
}

HashMap* hash_map_create(size_t value_size, size_t key_size, size_t (*hash)(const void*), int (*equals)(const void*, const void*)){
    HashMap* hash_map = malloc(sizeof(HashMap));
    hash_map->read_value_size = value_size;
    hash_map->read_key_size = key_size;
    if(value_size<sizeof(void*)){
        hash_map->value_size = sizeof(void*);
    }else{
        hash_map->value_size = value_size;
    }
    if(key_size<sizeof(void*)){
        hash_map->key_size = sizeof(void*);
    }else{
        hash_map->key_size = key_size;
    }

    hash_map->size = 0;
    hash_map->hash = hash;
    hash_map->equals = equals;
    for(size_t i=0; i<HASH_MAP_SIZE; i++){
        hash_map->arrays[i].arr = malloc(hash_map->value_size+hash_map->key_size);
        hash_map->arrays[i].alloc = 1;
        hash_map->arrays[i].size = 0;
    }

    return hash_map;
}

void hash_map_set_hash_function(HashMap* self, size_t (*hash)(const void*)){
    self->hash = hash;
}

void hash_map_set_equals_function(HashMap* self, int (*equals)(const void*, const void*)){
    self->equals = equals;
}

void hash_map_put(HashMap* self, const void* key, const void* value) {
    size_t index = hash(self, key);
    for(size_t i=0; i<self->arrays[index].size; i++){
        if(self->equals(
                INDEX_MAP_KEY(self, index, i),
                key
            )
        ){
            memcpy(INDEX_MAP_VALUE(self, index, i), value, self->read_value_size);
            return;
        }
    }

    if(self->arrays[index].size == self->arrays[index].alloc){
        self->arrays[index].alloc *= 2;
        self->arrays[index].arr = realloc(self->arrays[index].arr, self->arrays[index].alloc*(self->key_size+self->value_size));
    }

    memcpy(INDEX_MAP_KEY(self, index, self->arrays[index].size), key, self->read_key_size);
    memcpy(INDEX_MAP_VALUE(self, index, self->arrays[index].size), value, self->read_value_size);
    self->arrays[index].size++;
    self->size++;
}

void* hash_map_get(const HashMap* self, const void* key){
    size_t index = hash(self, key);
    for(size_t i=0; i<self->arrays[index].size; i++){
        if(self->equals(
                INDEX_MAP_KEY(self, index, i),
                key
            )
        ){
            return INDEX_MAP_VALUE(self, index, i);
        }
    }

    return NULL;
}

int hash_map_contains(const HashMap* self, const void* key){
    return hash_map_get(self, key) != NULL;
}

void hash_map_destroy(HashMap* self){
    for(size_t i=0; i<HASH_MAP_SIZE; i++){
        free(self->arrays[i].arr);
        self->arrays[i].arr = NULL;
        self->arrays[i].alloc = 0;
        self->arrays[i].size = 0;
    }
    self->size = 0;
}

size_t hash_map_size(HashMap* self){
    return self->size;
}

int hash_map_remove(HashMap* self, const void* key){
    size_t index = hash(self, key);
    size_t pos;
    int flag = 0;
    for(size_t i=0; i<self->arrays[index].size; i++){
        if(self->equals(
                INDEX_MAP_KEY(self, index, i),
                key
            )
        ){
            flag = 1;
            pos = i;
        }
        if(flag)
            break;
    }
    if(!flag){
        return 0;
    }

    const size_t block_size = self->key_size+self->value_size;
    const size_t move_size = (self->arrays[index].size - pos - 1)*block_size;
    if(move_size){
        memmove(INDEX_MAP_KEY(self, index, pos), INDEX_MAP_KEY(self, index, pos+1), (self->arrays[index].size - pos - 1)*block_size);
    }
    self->arrays[index].size--;
    self->size--;

    return 1;
}

void hash_map_clear(HashMap* self){
    for(size_t i=0; i<HASH_MAP_SIZE; i++){
        free(self->arrays[i].arr);
        self->arrays[i].arr = malloc(self->value_size+self->key_size);
        self->arrays[i].alloc = 1;
        self->arrays[i].size = 0;
    }
    self->size=0;
}

HashMapIterator* hash_map_iterate(HashMap* self){
    HashMapIterator* iterator = malloc(sizeof(HashMapIterator));
    iterator->hash_map = self;
    iterator->index = 0;
    iterator->pos = 0;
    
    if(iterator->hash_map->arrays[0].size){
        return iterator;
    }
    iterator_next(iterator);

    return iterator;
}

int iterator_next(HashMapIterator* self){
    if(self->index>=HASH_MAP_SIZE){
        return 0;
    }

    self->pos++;
    do{
        if(self->hash_map->arrays[self->index].size <= self->pos){
            self->index++;
            self->pos = 0;
        }else{
            return 1;
        }
    }while(self->index<HASH_MAP_SIZE);

    return 0;
}

void* iterator_key(HashMapIterator* self){
    if(self->index >= HASH_MAP_SIZE){
        return NULL;
    }else{
        return INDEX_MAP_KEY(self->hash_map, self->index, self->pos);
    }
}

void* iterator_value(HashMapIterator* self){
    if(self->index >= HASH_MAP_SIZE){
        return NULL;
    }else{
        return INDEX_MAP_VALUE(self->hash_map, self->index, self->pos);
    }
}

void iterator_destroy(HashMapIterator* self){
    free(self);
}
