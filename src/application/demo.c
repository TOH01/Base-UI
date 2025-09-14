#include "common.h"
#include "container.h"
#include "customButton.h"

void Calendar_InitUI(void) {

    AbsolutePos_t mainHeaderPos = {
        UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH)
    };
    
    container_t *mainHeader = windowAddContainer(mainHeaderPos);
    mainHeader->fixed = true;
    mainHeader->layout.right = LAYOUT_BORDER_RIGHT;
    mainHeader->layout.offsetRight = -400;
    mainHeader->fixedWidgets = true;

    buttonWidget_t * leftArrow = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);
    customButton_setButtonText(leftArrow, "<");
    containerAddWidgetAnchored(mainHeader, (BaseWidget_t*) leftArrow, (AbsolutePos_t){25, 0, 50, 75}, WIDGET_ANCHOR_LEFT, 10);

    buttonWidget_t * rightArrow = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);
    customButton_setButtonText(rightArrow, ">");
    containerAddWidgetAnchored(mainHeader, (BaseWidget_t*) rightArrow, (AbsolutePos_t){25, 0, 50, 75}, WIDGET_ANCHOR_RIGHT, 10);

    buttonWidget_t * currentMonth = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);
    customButton_setButtonText(currentMonth, "September 2025");
    containerAddWidgetAnchored(mainHeader, (BaseWidget_t*) currentMonth, (AbsolutePos_t){25, 0, 500, 75}, WIDGET_ANCHOR_CENTER, 0);

    AbsolutePos_t calendarPos = {
        UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)
    };
    container_t *calendarContainer = windowAddContainer(calendarPos);
    calendarContainer->fixed = true;
    calendarContainer->layout.right = LAYOUT_BORDER_RIGHT;
    calendarContainer->layout.bottom = LAYOUT_BORDER_BOTTOM;
    calendarContainer->layout.offsetRight = -400;

    AbsolutePos_t sidebarHeaderPos = {
        UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH)
    };

    container_t *sidebarHeader = windowAddContainer(sidebarHeaderPos);
    sidebarHeader->fixed = true;
    sidebarHeader->layout.right = LAYOUT_BORDER_RIGHT;
    sidebarHeader->layout.left = LAYOUT_BORDER_RIGHT;
    sidebarHeader->layout.offsetLeft = -400;

    buttonWidget_t * addGoal = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);
    customButton_setButtonText(addGoal, "Add Goal");
    containerAddWidgetAnchored(sidebarHeader, (BaseWidget_t*) addGoal, (AbsolutePos_t){25, 0, 250, 75}, WIDGET_ANCHOR_CENTER, 0);

    AbsolutePos_t sidebarContentPos = {
        UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)
    };
    container_t *sidebarContent = windowAddContainer(sidebarContentPos);
    sidebarContent->fixed = true;
    sidebarContent->layout.right = LAYOUT_BORDER_RIGHT;
    sidebarContent->layout.left = LAYOUT_BORDER_RIGHT;
    sidebarContent->layout.bottom = LAYOUT_BORDER_BOTTOM;
    sidebarContent->layout.offsetLeft = -400;
    
}

void Demo_InitAll(void) {
    Calendar_InitUI();
}