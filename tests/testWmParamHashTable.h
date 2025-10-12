#ifndef TEST_WM_PARAM_HASH_TABLE_H
#define TEST_WM_PARAM_HASH_TABLE_H

#include "WmParamHashTable.h"
#include "minunit.h"
#include "minwindef.h"

static WmParamTable_t* Table = NULL;
int Counter = 0;

/* Setup and teardown functions */
static void setup(void) {
    Counter = 0;
    Table = WmParamTable_Init();
    mu_check(Table != NULL);
}

static void teardown(void) {
    WmParamTable_Free(Table);
    Table = NULL;
}

LRESULT CountingHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    (void)hwnd;
    (void)msg;
    (void)wparam;
    (void)lparam;
    Counter++;
    return 0;
}

/* Tests */
MU_TEST(test_table_insert_null) {
    mu_assert_int_eq(-1, WmParamTable_Insert(NULL, 0, NULL));
    mu_assert_int_eq(0, Table->size);
}

MU_TEST(test_table_insert_full) {
    Table->size = MAX_WM_PARAM_AMOUNT + 1;
    mu_assert_int_eq(-1, WmParamTable_Insert(Table, 0, NULL));
}

MU_TEST(test_table_insert_param) {
    WmParamTable_Insert(Table, 1, &CountingHandler);
    mu_check(WmParamHandlerTable_IdHasHandler(Table, 1));
    mu_check(!WmParamHandlerTable_IdHasHandler(Table, 2));
}

MU_TEST(test_table_insert_duplicate_param) {
    WmParamTable_Insert(Table, 1, NULL);
    WmParamTable_Insert(Table, 1, NULL);

    // simulate hash function
    int expectedIdx = 1 % MAX_WM_PARAM_AMOUNT;

    // assume table is empty before, so can use calculated idx
    HandlerNode_t* first = Table->entries[expectedIdx].handlerList->firstHandlerNode;
    mu_check(first != NULL);
    mu_check(first->nextHandlerNode != NULL);
    mu_check(first != first->nextHandlerNode);
    mu_assert_int_eq(1, Table->size);
}

MU_TEST(test_table_call_handlers_of_id) {
    for (int i = 0; i < 10; i++) {
        WmParamTable_Insert(Table, 1, &CountingHandler);
    }
    WmParamHandlerTable_CallHandlersOfId(Table, NULL, 1, (WPARAM)0, (LPARAM)0);
    mu_assert_int_eq(10, Counter);
}

/* Test suite */
MU_TEST_SUITE(test_wm_param_hash_table) {
    MU_SUITE_CONFIGURE(setup, teardown);

    MU_RUN_TEST(test_table_insert_null);
    MU_RUN_TEST(test_table_insert_full);
    MU_RUN_TEST(test_table_insert_param);
    MU_RUN_TEST(test_table_insert_duplicate_param);
    MU_RUN_TEST(test_table_call_handlers_of_id);
}

#endif /* TEST_WM_PARAM_HASH_TABLE_H */
