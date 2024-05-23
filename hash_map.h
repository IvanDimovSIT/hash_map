#ifndef HASH_MAP_H
#define HASH_MAP_H
#include <stddef.h>

typedef struct HashMap HashMap;
typedef struct HashMapIterator HashMapIterator;

HashMap* hash_map_create(size_t value_size, size_t key_size, size_t (*hash)(const void*), int (*equals)(const void*, const void*));
void hash_map_set_hash_function(HashMap* self, size_t (*hash)(const void*));
void hash_map_set_equals_function(HashMap* self, int (*equals)(const void*, const void*));
void* hash_map_get(const HashMap* self, const void* key);
int hash_map_contains(const HashMap* self, const void* key);
void hash_map_put(HashMap* self, const void* key, const void* value);
void hash_map_destroy(HashMap* self);
size_t hash_map_size(HashMap* self);
int hash_map_remove(HashMap* self, const void* key);
void hash_map_clear(HashMap* self);
HashMapIterator* hash_map_iterate(HashMap* self);

void iterator_destroy(HashMapIterator* self);
int iterator_next(HashMapIterator* self);
void* iterator_key(HashMapIterator* self);
void* iterator_value(HashMapIterator* self);

#endif
