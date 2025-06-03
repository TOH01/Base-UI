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
	return NULL;
}

void DynamicArray_Insert(DynamicArray_t * array, void * data, int idx){
	if(idx < array->size){
		array->items[idx] = data;
	}
}

void DynamicArray_Swap(DynamicArray_t * array, int idx1, int idx2){
	void * temp = DynamicArray_get(array, idx2);
	
	DynamicArray_Insert(array, DynamicArray_get(array, idx1), idx2);
	DynamicArray_Insert(array, temp, idx1);
}

void DynamicArray_Free(DynamicArray_t *array) {
    if (!array || !array->items) return;

    for (int i = 0; i < array->size; ++i) {
        free(array->items[i]); 
    }

    free(array->items);  
    array->items = NULL;
    array->size = 0;
    array->capacity = 0;
}