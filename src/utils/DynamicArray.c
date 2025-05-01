#include <stdlib.h>
#include "DynamicArray.h"

DynamicArray_t * DynamicArray_init(int initialCapacity){
	DynamicArray_t * array = (DynamicArray_t *) calloc(1, sizeof(DynamicArray_t));
	array->capacity = initialCapacity;
	array->size = 0;
	array->items = calloc(initialCapacity, sizeof(void *));
	return array;
}

void DynamicArray_Add(DynamicArray_t * array, void * data){
	
	if(array->size == array->capacity){
		array->items = realloc(array->items, (array->capacity * 2) * sizeof(void *));
		array->capacity = array->capacity * 2;
	}
	
	array->items[array->size] = data;
	array->size++;
}

void * DynamicArray_get(DynamicArray_t * array, int idx){
	if(idx < array->size){
		return array->items[idx];
	}
}