// disable internal debug from modules
#undef DEBUG

#include "minunit.h"
#include "testDynamicArray.h"
#include "testUiUtils.h"
#include "testWmParamHashTable.h"

int main(void) {
    MU_RUN_SUITE(test_dynamic_array_suite);
    MU_RUN_SUITE(test_ui_utils_suite);
    MU_RUN_SUITE(test_wm_param_hash_table);
    MU_REPORT();
    return 0;
}