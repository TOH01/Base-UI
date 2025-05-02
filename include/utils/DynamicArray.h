#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

typedef struct DynamicArray {
	void ** items;
	int size;
	int capacity;
} DynamicArray_t;

void * DynamicArray_get(DynamicArray_t * array, int idx);
void DynamicArray_Add(DynamicArray_t * array, void * data);
DynamicArray_t * DynamicArray_init(int initialCapacity);
void DynamicArray_Insert(DynamicArray_t * array, void * data, int idx);

#endif