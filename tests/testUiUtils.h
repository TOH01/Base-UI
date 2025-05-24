#ifndef TEST_UI_UTILS_H
#define TEST_UI_UTILS_H

#include "minunit.h"
#include "UiUtils.h" // Include the header where getPosToContainer is defined

MU_TEST(test_UiUtils_absolutePosToRect_basic) {
    AbsolutePos_t input = { .top = 10, .left = 20, .right = 120, .bottom = 60 };
    RECT rect = UiUtils_absolutePosToRect(input);

    mu_assert_int_eq(20, rect.left);
    mu_assert_int_eq(10, rect.top);
    mu_assert_int_eq(120, rect.right);
    mu_assert_int_eq(60, rect.bottom);
}

MU_TEST(test_coordinate_inside_box) {
    AbsolutePos_t pos = { .left = 100, .top = 100, .right = 200, .bottom = 200 };

    mu_check(UiUtils_CoordinateIsInsideOf(150, 150, pos));  // inside
    mu_check(!UiUtils_CoordinateIsInsideOf(99, 150, pos));  // left
    mu_check(!UiUtils_CoordinateIsInsideOf(201, 150, pos)); // right
    mu_check(!UiUtils_CoordinateIsInsideOf(150, 99, pos));  // top
    mu_check(!UiUtils_CoordinateIsInsideOf(150, 201, pos)); // bottom
    mu_check(!UiUtils_CoordinateIsInsideOf(100, 100, pos)); // on border
    mu_check(!UiUtils_CoordinateIsInsideOf(200, 200, pos)); // on border
}

AbsolutePos_t testPos = {100, 50, 200, 150}; // left=100, top=50, right=200, bottom=150
int borderWidth = 5;

MU_TEST(test_left_border) {
    int x = 102; // within 2px of left border
    int y = 100; // inside vertical bounds
    mu_assert(UiUtils_CoordinateIsOnBorderOf(x, y, borderWidth, testPos) == LEFT, "Should be left Border");
}

MU_TEST(test_right_border) {
    int x = 198; // within 2px of right border
    int y = 100; 
    mu_assert(UiUtils_CoordinateIsOnBorderOf(x, y, borderWidth, testPos) == RIGHT, "Should be right Border");
}

MU_TEST(test_top_border) {
    int x = 150;
    int y = 52; // within 2px of top border
    mu_assert(UiUtils_CoordinateIsOnBorderOf(x, y, borderWidth, testPos) == TOP, "Should be top Border");
}

MU_TEST(test_bottom_border) {
    int x = 150;
    int y = 148; // within 2px of bottom border
    mu_assert(UiUtils_CoordinateIsOnBorderOf(x, y, borderWidth, testPos) == BOTTOM, "Should be bottom Border");
}

MU_TEST(test_outside_border) {
    int x = 300;
    int y = 300;
    mu_assert(UiUtils_CoordinateIsOnBorderOf(x, y, borderWidth, testPos) == BORDER_NONE, "Should be no Border");
}

AbsolutePos_t container = {100, 100, 300, 300}; // left, top, right, bottom

MU_TEST(test_widget_fully_inside) {
    AbsolutePos_t widget = {150, 150, 250, 250};
    mu_check(UiUtils_WidgetFitsInContainer(widget, container));
}

MU_TEST(test_widget_partial_outside_left) {
    AbsolutePos_t widget = {90, 150, 250, 250};
    mu_check(!UiUtils_WidgetFitsInContainer(widget, container));
}

MU_TEST(test_widget_partial_outside_right) {
    AbsolutePos_t widget = {150, 150, 310, 250};
    mu_check(!UiUtils_WidgetFitsInContainer(widget, container));
}

MU_TEST(test_widget_partial_outside_top) {
    AbsolutePos_t widget = {150, 90, 250, 250};
    mu_check(!UiUtils_WidgetFitsInContainer(widget, container));
}

MU_TEST(test_widget_partial_outside_bottom) {
    AbsolutePos_t widget = {150, 150, 250, 310};
    mu_check(!UiUtils_WidgetFitsInContainer(widget, container));
}

MU_TEST_SUITE(test_ui_utils_suite) {
    MU_RUN_TEST(test_UiUtils_absolutePosToRect_basic);
    MU_RUN_TEST(test_coordinate_inside_box);
    MU_RUN_TEST(test_left_border);
    MU_RUN_TEST(test_right_border);
    MU_RUN_TEST(test_top_border);
    MU_RUN_TEST(test_bottom_border);
    MU_RUN_TEST(test_outside_border);
    MU_RUN_TEST(test_widget_fully_inside);
    MU_RUN_TEST(test_widget_partial_outside_left);
    MU_RUN_TEST(test_widget_partial_outside_right);
    MU_RUN_TEST(test_widget_partial_outside_top);
    MU_RUN_TEST(test_widget_partial_outside_bottom);
}

#endif