#include <stdio.h>
#include "./hash_map.h"

size_t hash(const void* src){
	int value = *((int*)src);
	if(value<0){
		value = -value;
	}
	return (value/10)%10;
}

int equals(const void* a, const void* b){
	int v1 = *((int*)a);
	int v2 = *((int*)b);

	return v1 == v2;
}

int main(){
	HashMap* map = hash_map_create(sizeof(double), sizeof(int), hash, equals);
	int key = 10;
	double value = 1.22;
	hash_map_put(map, &key, &value);
	key = 1340;
	value = 3.0;
	hash_map_put(map, &key, &value);


	key = 1341;
	value = 5.5;
	hash_map_put(map, &key, &value);

	double* found = hash_map_get(map, &key);
	printf("%lf == %lf\n", value, *(found));
	hash_map_remove(map, &key);
	found = hash_map_get(map, &key);
	printf("NULL == %p\n", found);

	key = 4;
	value = -11.22;
	hash_map_put(map, &key, &value);

	HashMapIterator* iter = hash_map_iterate(map);
	do{
		printf("%d : %lf\n", *((int*)iterator_key(iter)), *((double*)iterator_value(iter)));
	}while(iterator_next(iter));

	iterator_destroy(iter);
	hash_map_destroy(map);
	return 0;
}
