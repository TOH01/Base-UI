#include "DynamicArray.h"
#include <stdlib.h>

DynamicArray_t *DynamicArray_init(int initialCapacity) {
	DynamicArray_t *array = (DynamicArray_t *)calloc(1, sizeof(DynamicArray_t));
	array->capacity = initialCapacity;
	array->size = 0;
	array->items = calloc(initialCapacity, sizeof(void *));
	return array;
}

void DynamicArray_Add(DynamicArray_t *array, void *data) {
	if (array->size == array->capacity) {
		array->items = realloc(array->items, (array->capacity * 2) * sizeof(void *));
		array->capacity = array->capacity * 2;
	}

	array->items[array->size] = data;
	array->size++;
}

void *DynamicArray_get(DynamicArray_t *array, int idx) {
	if (idx < array->size) {
		return array->items[idx];
	}
	return NULL;
}

void DynamicArray_Insert(DynamicArray_t *array, void *data, int idx) {
	if (idx < array->size) {
		array->items[idx] = data;
	}
}

void DynamicArray_Swap(DynamicArray_t *array, int idx1, int idx2) {
	void *temp = DynamicArray_get(array, idx2);

	DynamicArray_Insert(array, DynamicArray_get(array, idx1), idx2);
	DynamicArray_Insert(array, temp, idx1);
}

void DynamicArray_Free(DynamicArray_t *array) {
	for (int i = 0; i < array->size; ++i) {
		free(array->items[i]);
	}

	free(array->items);
	free(array);
}

/**
 * @brief Removes the item at the specified index from the dynamic array.
 *
 * This function shifts all subsequent items left by one position,
 * effectively removing the item at the given index. The array's size
 * is reduced by one.
 *
 * @param array Pointer to the dynamic array.
 * @param idx   Index of the item to remove (0-based).
 *
 * @warning This function does NOT free the memory of the item being removed.
 *          If the array holds dynamically allocated memory, you must manually
 *          free the item at `array->items[idx]` before calling this function
 *          to avoid memory leaks.
 *
 * @note Invalid index or NULL pointers are silently ignored.
 */
void DynamicArray_RemoveAt(DynamicArray_t *array, int idx) {
	for (int i = idx; i < array->size - 1; i++) {
		array->items[i] = array->items[i + 1];
	}
	array->size--;
}