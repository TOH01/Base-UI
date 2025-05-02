#ifndef TEST_DYNAMIC_ARRAY_H
#define TEST_DYNAMIC_ARRAY_H

#include "minunit.h"
#include "DynamicArray.h"
#include <stdlib.h>

MU_TEST(test_DynamicArray_init) {
    // Test initialization of DynamicArray
    DynamicArray_t *array = DynamicArray_init(10);

    mu_assert(array != NULL, "DynamicArray_init failed to allocate memory");
    mu_assert(array->capacity == 10, "Initial capacity should be 10");
    mu_assert(array->size == 0, "Initial size should be 0");

    free(array->items);
    free(array);
}

MU_TEST(test_DynamicArray_Add) {
    // Test adding elements to the DynamicArray
    DynamicArray_t *array = DynamicArray_init(2);  // Start with a small capacity to test resizing

    int data1 = 1, data2 = 2, data3 = 3;

    DynamicArray_Add(array, &data1);
    mu_assert(array->size == 1, "Size should be 1 after adding an element");
    mu_assert(*(int *)array->items[0] == 1, "First element should be 1");

    DynamicArray_Add(array, &data2);
    mu_assert(array->size == 2, "Size should be 2 after adding another element");
    mu_assert(*(int *)array->items[1] == 2, "Second element should be 2");

    // Adding a third element will trigger a resize
    DynamicArray_Add(array, &data3);
    mu_assert(array->size == 3, "Size should be 3 after adding third element");
    mu_assert(array->capacity == 4, "Capacity should have doubled to 4");

    free(array->items);
    free(array);
}

MU_TEST(test_DynamicArray_get) {
    // Test getting elements from the DynamicArray
    DynamicArray_t *array = DynamicArray_init(2);
    
    int data1 = 1, data2 = 2;
    DynamicArray_Add(array, &data1);
    DynamicArray_Add(array, &data2);

    // Test valid indices
    mu_assert(*(int *)DynamicArray_get(array, 0) == 1, "First element should be 1");
    mu_assert(*(int *)DynamicArray_get(array, 1) == 2, "Second element should be 2");

    // Test invalid index (out of bounds)
    mu_assert(DynamicArray_get(array, 2) == NULL, "Accessing out-of-bounds index should return NULL");

    free(array->items);
    free(array);
}

MU_TEST(test_DynamicArray_Insert) {
    DynamicArray_t *array = DynamicArray_init(2);

    int a = 1, b = 2;
    DynamicArray_Add(array, &a); // index 0
    DynamicArray_Add(array, &b); // index 1

    int newValue = 42;
    DynamicArray_Insert(array, &newValue, 1); // overwrite index 1

    mu_assert_int_eq(2, array->size); // size shouldn't change
    mu_assert_int_eq(42, *(int*) DynamicArray_get(array, 1)); // should be overwritten

    free(array->items);
    free(array);
}

MU_TEST_SUITE(test_dynamic_array_suite) {
    MU_RUN_TEST(test_DynamicArray_init);
    MU_RUN_TEST(test_DynamicArray_Add);
    MU_RUN_TEST(test_DynamicArray_get);
    MU_RUN_TEST(test_DynamicArray_Insert);
}

#endif