#include "DynamicArray.h"
#include "UiTheme.h"
#include "calendar.h"
#include "calendar_entry.h"
#include "common.h"
#include "container.h"
#include "customButton.h"
#include "customCheckbox.h"
#include "customInput.h"
#include "menu.h"
#include "persistance.h"
#include <stdio.h>

int activeYear = 2025;
int activeMonth = 9;
bool value = false;
bool isNumericGoal = true;
bool isBooleanGoal = false;

ButtonTheme_t borderlessButton;

calendarWidget_t *calendar;
buttonWidget_t *currentMonth;
container_t *sidebarContent;

submenuGroup_t *addGoalGroup;
MenuUi_Submenu_t *addGoalMenu;
MenuUi_Submenu_t *ghostMenu;

checkboxWidget_t *numericCheckbox;
checkboxWidget_t *booleanCheckbox;

inputWidget_t *yearInput;
inputWidget_t *monthInput;
inputWidget_t *dayInput;
inputWidget_t *nameInput;

day_save_data_t *day;

char *monthNames[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

char addGoalStr[30] = "Add goal";
char xButtonStr[30] = "X";

static void numericCheckboxCallback(void) {
	if (isBooleanGoal && isNumericGoal) {
		isBooleanGoal = false;
	}
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

static void booleanCheckboxCallback(void) {
	if (isBooleanGoal && isNumericGoal) {
		isNumericGoal = false;
	}
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

static void updateTitle(void) {
	char buf[50];
	sprintf(buf, "%s %d", monthNames[activeMonth - 1], activeYear);
	customButton_setButtonText(currentMonth, buf);
}

static void onDataUpdate(void);

static void calendarDayChange(void) {
	int day_num = calendar->selectedDay;
	int month = calendar->month;
	int year = calendar->year;

	if (day != NULL) {
		free(day->entries);
		free(day);
		day = NULL;
	}

	destroyContainerContent(sidebarContent);

	day = loadDay(day_num, month, year);

	if (day) {
		renderCalendarEntries(sidebarContent, day->entries, day->elements, &onDataUpdate);
	}
}

static void onDataUpdate(void) {
	if (activeInput == NULL) {
		overwriteDayData(day, calendar->selectedDay, calendar->month, calendar->year);
		calendarDayChange();
	}
}

static void saveButtonCallback(int id) {
	(void)id;

	int day_num = atoi(dayInput->buffer);
	int month = atoi(monthInput->buffer);
	int year = atoi(yearInput->buffer);

	calender_entry_t entry;
	memset(&entry, 0, sizeof(entry));

	strncpy(entry.text, nameInput->buffer, sizeof(entry.text) - 1);
	entry.text[sizeof(entry.text) - 1] = '\0';

	if (*(numericCheckbox->value)) {
		entry.type = ENTRY_NUM;
	} else {
		entry.type = ENTRY_CHECKBOX;
	}

	day_save_data_t day;
	day.elements = 1;
	day.entries = &entry;

	saveDay(&day, day_num, month, year);

	calendarDayChange();
}

static void leftArrowCallback(int id) {
	(void)id;
	if (activeMonth == 1) {
		activeMonth = 12;
		activeYear--;
	} else {
		activeMonth--;
	}
	updateVisibleCalendar(calendar, activeYear, activeMonth);
	updateTitle();
}

static void rightArrowCallback(int id) {
	(void)id;
	if (activeMonth == 12) {
		activeMonth = 1;
		activeYear++;
	} else {
		activeMonth++;
	}
	updateVisibleCalendar(calendar, activeYear, activeMonth);
	updateTitle();
}

void Calendar_InitUI(void) {

	borderlessButton = currentWindowState.activeTheme.button;
	borderlessButton.borderWidth = 0;

	AbsolutePos_t mainHeaderPos = {UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.02, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH)};

	container_t *mainHeader = windowAddContainer(mainHeaderPos);
	mainHeader->fixed = true;
	mainHeader->layout.right = LAYOUT_BORDER_RIGHT;
	mainHeader->layout.offsetRight = -425;
	mainHeader->fixedWidgets = true;
	mainHeader->disableRectRender = true;

	buttonWidget_t *leftArrow = customButton_initButton((CommonPos_t){0, 0, 0, 0}, &leftArrowCallback, 0);
	customButton_setButtonText(leftArrow, "<");
	leftArrow->theme = &borderlessButton;
	containerAddWidgetAnchored(mainHeader, (BaseWidget_t *)leftArrow, (AbsolutePos_t){25, 0, 50, 75}, WIDGET_ANCHOR_LEFT, 0);

	buttonWidget_t *rightArrow = customButton_initButton((CommonPos_t){0, 0, 0, 0}, &rightArrowCallback, 0);
	customButton_setButtonText(rightArrow, ">");
	rightArrow->theme = &borderlessButton;
	containerAddWidgetAnchored(mainHeader, (BaseWidget_t *)rightArrow, (AbsolutePos_t){25, 0, 50, 75}, WIDGET_ANCHOR_RIGHT, 0);

	currentMonth = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);
	customButton_setButtonText(currentMonth, "September 2025");
	currentMonth->theme = &borderlessButton;
	containerAddWidgetAnchored(mainHeader, (BaseWidget_t *)currentMonth, (AbsolutePos_t){25, 0, 500, 75}, WIDGET_ANCHOR_CENTER, 0);

	AbsolutePos_t calendarPos = {UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.02, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)};
	container_t *calendarContainer = windowAddContainer(calendarPos);
	calendarContainer->fixed = true;
	calendarContainer->layout.right = LAYOUT_BORDER_RIGHT;
	calendarContainer->layout.bottom = LAYOUT_BORDER_BOTTOM;
	calendarContainer->layout.offsetRight = -425;
	calendarContainer->layout.offsetBottom = -50;
	calendarContainer->disableRectRender = true;

	calendar = initCalendarWidget((CommonPos_t){0, 0, 1, 1}, activeYear, activeMonth);
	calendar->dateChangeCallback = &calendarDayChange;
	containerAddWidget(calendarContainer, (BaseWidget_t *)calendar);

	AbsolutePos_t sidebarHeaderPos = {UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH)};

	container_t *sidebarHeader = windowAddContainer(sidebarHeaderPos);
	sidebarHeader->fixed = true;
	sidebarHeader->layout.right = LAYOUT_BORDER_RIGHT;
	sidebarHeader->layout.left = LAYOUT_BORDER_RIGHT;
	sidebarHeader->layout.offsetLeft = -400;
	sidebarHeader->layout.offsetRight = -50;
	sidebarHeader->disableRectRender = true;

	buttonWidget_t *addGoal = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);
	customButton_setButtonText(addGoal, "Add Goal");
	containerAddWidgetAnchored(sidebarHeader, (BaseWidget_t *)addGoal, (AbsolutePos_t){25, 0, 350, 75}, WIDGET_ANCHOR_CENTER, 0);

	AbsolutePos_t sidebarContentPos = {UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.75, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)};
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

	addGoalGroup = initSubmenuGroup();
	addGoalMenu = MenuUi_SubmenuInit(addGoalStr, addGoal, addGoalGroup);

	container_t *addGoalContainer = windowAddGridContainer((AbsolutePos_t){100, 100, 400, 400}, 10, 10);

	MenuUi_SubmenuAddContainer(addGoalMenu, addGoalContainer);
	addGoalContainer->movable = true;

	buttonWidget_t *exitButton = customButton_initButton((CommonPos_t){0, 0.9, 1, 0.1}, NULL, 0);
	addWidgetToGridContainer(addGoalContainer, (BaseWidget_t *)exitButton, 0, 9);

	ghostMenu = MenuUi_SubmenuInit(xButtonStr, exitButton, addGoalGroup);
	MenuUi_SetSubmenuActive(ghostMenu);

	nameInput = customInput_initInput((CommonPos_t){0, 0, 0, 0});
	addWidgetToGridContainerSpan(addGoalContainer, (BaseWidget_t *)nameInput, 1, 1, 2, 7);

	setDefaultText(nameInput, "Goal Name");

	yearInput = customInput_initInput((CommonPos_t){0, 0, 0, 0});
	monthInput = customInput_initInput((CommonPos_t){0, 0, 0, 0});
	dayInput = customInput_initInput((CommonPos_t){0, 0, 0, 0});
	addWidgetToGridContainerSpan(addGoalContainer, (BaseWidget_t *)dayInput, 3, 3, 1, 1);
	addWidgetToGridContainerSpan(addGoalContainer, (BaseWidget_t *)monthInput, 3, 3, 3, 3);
	addWidgetToGridContainerSpan(addGoalContainer, (BaseWidget_t *)yearInput, 3, 3, 5, 8);

	setDefaultText(yearInput, "YYYY");
	setDefaultText(monthInput, "MM");
	setDefaultText(dayInput, "DD");

	numericCheckbox = customCheckbox_initCheckbox((CommonPos_t){0, 0, 0, 0}, &isNumericGoal);
	booleanCheckbox = customCheckbox_initCheckbox((CommonPos_t){0, 0, 0, 0}, &isBooleanGoal);
	buttonWidget_t *numericCheckboxButton = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);
	buttonWidget_t *booleanCheckboxButton = customButton_initButton((CommonPos_t){0, 0, 0, 0}, NULL, 0);

	customCheckbox_setStateChangeCallback(numericCheckbox, &numericCheckboxCallback);
	customCheckbox_setStateChangeCallback(booleanCheckbox, &booleanCheckboxCallback);

	addWidgetToGridContainerSpan(addGoalContainer, (BaseWidget_t *)numericCheckbox, 5, 5, 6, 6);
	addWidgetToGridContainerSpan(addGoalContainer, (BaseWidget_t *)booleanCheckbox, 6, 6, 6, 6);
	addWidgetToGridContainerSpan(addGoalContainer, (BaseWidget_t *)numericCheckboxButton, 5, 5, 1, 4);
	addWidgetToGridContainerSpan(addGoalContainer, (BaseWidget_t *)booleanCheckboxButton, 6, 6, 1, 4);
	customButton_setButtonText(numericCheckboxButton, "Numeric Goal");
	customButton_setButtonText(booleanCheckboxButton, "Boolean Goal");
	numericCheckboxButton->theme = &borderlessButton;
	booleanCheckboxButton->theme = &borderlessButton;

	buttonWidget_t *saveButton = customButton_initButton((CommonPos_t){0, 0, 0, 0}, &saveButtonCallback, 0);
	customButton_setButtonText(saveButton, "Add");

	addWidgetToGridContainerSpan(addGoalContainer, (BaseWidget_t *)saveButton, 8, 8, 3, 6);
}

void Demo_InitAll(void) {
	create_file_system();
	Calendar_InitUI();

	setOnActiveInputEndCbk(&onDataUpdate);
}