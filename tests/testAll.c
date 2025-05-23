#include "testUiUtils.h"
#include "testDynamicArray.h"

int main() {
    MU_RUN_SUITE(test_dynamic_array_suite);
    MU_RUN_SUITE(test_ui_utils_suite);
    MU_REPORT();
    return 0;
}