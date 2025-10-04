#include "testDynamicArray.h"
#include "testUiUtils.h"

int main(void) {
    MU_RUN_SUITE(test_dynamic_array_suite);
    MU_RUN_SUITE(test_ui_utils_suite);
    MU_REPORT();
    return 0;
}