#include "common.h"
#include "container.h"
#include "customButton.h"
#include "calendar.h"
#include <stdio.h>
#include "customCheckbox.h"
#include "customInput.h"
#include "UiTheme.h"
#include "calendar_entry.h"
#include "persistance.h"

calender_entry_t entries[] = {
    { ENTRY_CHECKBOX, "Did Workout", { .state = true } },
    { ENTRY_NUM,      "Water (ml)",  { .count = 500 } },
    { ENTRY_CHECKBOX, "Take Vitamins", { .state = false } },
};

int activeYear = 2025;
int activeMonth = 9;
bool value = false;

ButtonTheme_t borderlessButton;

calendarWidget_t * calendar;
buttonWidget_t * currentMonth;
container_t *sidebarContent;

char * monthNames[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

char goal1[] = {"Pushups"};
char goal2[] = {"Go out"};

static void updateTitle(void){
    char buf[50];
    sprintf(buf, "%s %d", monthNames[activeMonth-1], activeYear);
    customButton_setButtonText(currentMonth, buf);
}

static void calendarDayChange(void){}

static void leftArrowCallback(int id){
    (void) id;
    if(activeMonth == 1){
        activeMonth = 12;
        activeYear--;
    }
    else{
        activeMonth--;
    }
    updateVisibleCalendar(calendar, activeYear, activeMonth);
    updateTitle();
}

static void rightArrowCallback(int id){
    (void) id;
    if(activeMonth == 12){
        activeMonth = 1;
        activeYear++;
    }
    else{
        activeMonth++;
    }
    updateVisibleCalendar(calendar, activeYear, activeMonth);
    updateTitle();
}

void Calendar_InitUI(void) {

    borderlessButton = currentWindowState.activeTheme.button;
    borderlessButton.borderWidth = 0;

    AbsolutePos_t mainHeaderPos = {
        UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.02, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH)
    };
    
    container_t *mainHeader = windowAddContainer(mainHeaderPos);
    mainHeader->fixed = true;
    mainHeader->layout.right = LAYOUT_BORDER_RIGHT;
    mainHeader->layout.offsetRight = -425;
    mainHeader->fixedWidgets = true;
    mainHeader->disableRectRender = true;

    buttonWidget_t * leftArrow = customButton_initButton((CommonPos_t){0, 0, 0, 0}, &leftArrowCallback, 0);
    customButton_setButtonText(leftArrow, "<");
    leftArrow->theme = &borderlessButton;
    containerAddWidgetAnchored(mainHeader, (BaseWidget_t*) leftArrow, (AbsolutePos_t){25, 0, 50, 75}, WIDGET_ANCHOR_LEFT, 0);

    buttonWidget_t * rightArrow = customButton_initButton((CommonPos_t){0, 0, 0, 0}, &rightArrowCallback, 0);
    customButton_setButtonText(rightArrow, ">");
    rightArrow->theme = &borderlessButton;
    containerAddWidgetAnchored(mainHeader, (BaseWidget_t*) rightArrow, (AbsolutePos_t){25, 0, 50, 75}, WIDGET_ANCHOR_RIGHT, 0);

    currentMonth = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);
    customButton_setButtonText(currentMonth, "September 2025");
    currentMonth->theme = &borderlessButton;
    containerAddWidgetAnchored(mainHeader, (BaseWidget_t*) currentMonth, (AbsolutePos_t){25, 0, 500, 75}, WIDGET_ANCHOR_CENTER, 0);

    AbsolutePos_t calendarPos = {
        UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.02, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)
    };
    container_t *calendarContainer = windowAddContainer(calendarPos);
    calendarContainer->fixed = true;
    calendarContainer->layout.right = LAYOUT_BORDER_RIGHT;
    calendarContainer->layout.bottom = LAYOUT_BORDER_BOTTOM;
    calendarContainer->layout.offsetRight = -425;
    calendarContainer->layout.offsetBottom = -50;
    calendarContainer->disableRectRender = true;

    calendar = initCalendarWidget((CommonPos_t){0, 0, 1, 1}, activeYear, activeMonth);
    calendar->dateChangeCallback = &calendarDayChange;
    containerAddWidget(calendarContainer, (BaseWidget_t*) calendar);

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
    sidebarHeader->layout.offsetRight = -50;
    sidebarHeader->disableRectRender = true;

    buttonWidget_t * addGoal = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);
    customButton_setButtonText(addGoal, "Add Goal");
    containerAddWidgetAnchored(sidebarHeader, (BaseWidget_t*) addGoal, (AbsolutePos_t){25, 0, 350, 75}, WIDGET_ANCHOR_CENTER, 0);

    AbsolutePos_t sidebarContentPos = {
        UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH),
        UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_WIDTH),
        UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)
    };
    sidebarContent = windowAddGridContainer(sidebarContentPos, 20, 10);
    sidebarContent->fixed = true;
    sidebarContent->fixedWidgets = true;
    sidebarContent->layout.right = LAYOUT_BORDER_RIGHT;
    sidebarContent->layout.left = LAYOUT_BORDER_RIGHT;
    sidebarContent->layout.bottom = LAYOUT_BORDER_BOTTOM;
    sidebarContent->layout.offsetLeft = -400;
    sidebarContent->layout.offsetRight = -50;
    sidebarContent->layout.offsetBottom = -50;
    sidebarContent->limitGrid = true;
    sidebarContent->cellMaxHeight = 75;
    sidebarContent->cellMinHeight = 75;
    sidebarContent->cellMaxWidth = 9999;
    sidebarContent->cellMinWidth = 0;

    int numEntries = sizeof(entries)/sizeof(entries[0]);

    renderCalendarEntries(sidebarContent, entries, numEntries);
 
}

void Demo_InitAll(void) {
    create_file_system();
    Calendar_InitUI();
}