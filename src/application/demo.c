#include "colorMatrix.h"
#include "common.h"
#include "customButton.h"
#include "customCheckbox.h"
#include "customInput.h"
#include "customSlider.h"
#include "drawable.h"
#include "menu.h"
#include "naricaMap.h"
#include "resource.h"
#include "textDump.h"
#include "widget.h"
#include <stdio.h>

buttonWidget_t *textDumpMenuButtons[3];
const char *textDumpDefaultLines[] = {
    "Euclidian distance, generate a path!",
    "March time, generate a path!",
    "Water cost, generate a path!",
};

const char *textDumpMenuButtonNames[] = {
    "Distance",
    "Time",
    "Water",
};

#define PREV_PATH_ID -1
#define NEXT_PATH_ID 1

const char colorMatrixText[] = "Path color";
const char generatePathText[] = "Generate path";
const char previousPathText[] = "< Previous path";
const char nextPathText[] = "Next path >";
const char deleteText[] = "Delete path";
const char clearSelectionText[] = "Clear selection";
const char generateFriendlyTownText[] = "homebase +";
const char generateEnemeyTownText[] = "Enemy homebase +";
const char tileSearchLabelText[] = "Tile finder";

container_t *textDumpContainers[3];
textDumpWidget_t *textDumps[3];

submenuGroup_t *textDumpMenuGroup;
MenuUi_Submenu_t *textDumpSubmenus[3];

narciaMap_t *narciaMap;
colorMatrix_t *colorMatrix;

inputWidget_t *inputX;
inputWidget_t *inputY;

int selectedPathIdx = 0;

LRESULT textDumpWaterMenuCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)hwnd;
	(void)msg;
	(void)wParam;
	(void)lParam;
	customButton_setTo3SliceBackground(textDumpMenuButtons[0], IDI_LEFT_BTN_2, IDI_CENTER_BTN_2, IDI_RIGHT_BTN_2);
	customButton_setTo3SliceBackground(textDumpMenuButtons[1], IDI_LEFT_BTN_2, IDI_CENTER_BTN_2, IDI_RIGHT_BTN_2);
	customButton_setTo3SliceBackground(textDumpMenuButtons[2], IDI_LEFT_BTN_2_ACTIVE, IDI_CENTER_BTN_2_ACTIVE, IDI_RIGHT_BTN_2_ACTIVE);
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
	return 0;
}

LRESULT textDumpTimeMenuCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)hwnd;
	(void)msg;
	(void)wParam;
	(void)lParam;
	customButton_setTo3SliceBackground(textDumpMenuButtons[0], IDI_LEFT_BTN_2, IDI_CENTER_BTN_2, IDI_RIGHT_BTN_2);
	customButton_setTo3SliceBackground(textDumpMenuButtons[2], IDI_LEFT_BTN_2, IDI_CENTER_BTN_2, IDI_RIGHT_BTN_2);
	customButton_setTo3SliceBackground(textDumpMenuButtons[1], IDI_LEFT_BTN_2_ACTIVE, IDI_CENTER_BTN_2_ACTIVE, IDI_RIGHT_BTN_2_ACTIVE);
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
	return 0;
}

LRESULT textDumpDistanceMenuCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)hwnd;
	(void)msg;
	(void)wParam;
	(void)lParam;
	customButton_setTo3SliceBackground(textDumpMenuButtons[2], IDI_LEFT_BTN_2, IDI_CENTER_BTN_2, IDI_RIGHT_BTN_2);
	customButton_setTo3SliceBackground(textDumpMenuButtons[1], IDI_LEFT_BTN_2, IDI_CENTER_BTN_2, IDI_RIGHT_BTN_2);
	customButton_setTo3SliceBackground(textDumpMenuButtons[0], IDI_LEFT_BTN_2_ACTIVE, IDI_CENTER_BTN_2_ACTIVE, IDI_RIGHT_BTN_2_ACTIVE);
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
	return 0;
}

void pathSelectButtonCallback(int id) {
	if (narciaMap->paths->size) {
		if (selectedPathIdx + id < 0) {
			selectedPathIdx = narciaMap->paths->size - 1;
		} else if (selectedPathIdx + id > narciaMap->paths->size - 1) {
			selectedPathIdx = 0;
		} else {
			selectedPathIdx = selectedPathIdx + id;
		}
		narciaMap->selecetdPath = (path_t *)DynamicArray_get(narciaMap->paths, selectedPathIdx);
		goToTile(narciaMap, narciaMap->selecetdPath->tiles[0].x, narciaMap->selecetdPath->tiles[0].y);
	}
}

void clearSelection(int id) {
	(void)id;
	narciaMap->selecetdPath = NULL;
	narciaMap->selected1 = (Coordinate_t){-1, -1};
	narciaMap->selected2 = (Coordinate_t){-1, -1};
}

void deletePathCallback(int id) {
	(void)id;

	if (narciaMap->selecetdPath != NULL) {
		path_t *path = DynamicArray_get(narciaMap->paths, selectedPathIdx);
		free(path->tiles);
		free(path);
		DynamicArray_RemoveAt(narciaMap->paths, selectedPathIdx);

		InvalidateRect(currentWindowState.hwnd, NULL, FALSE);

		narciaMap->selecetdPath = NULL;
	}
}

void goButtonCallback(int id) {
	(void)id;

	char *endX;
	char *endY;

	int x = (int)strtol(inputX->buffer, &endX, 10);
	int y = (int)strtol(inputY->buffer, &endY, 10);

	if (*endX == '\0' && *endY == '\0') {
		if (x < narciaMap->mapSize && y < narciaMap->mapSize) {
			goToTile(narciaMap, x, y);
		}
	}
}

void generatePath(int id) {
	(void)id;

	path_t *path = findShortestPath(narciaMap, narciaMap->selected1, narciaMap->selected2);

	if (colorMatrixHasActive(colorMatrix)) {
		path->color = colorMatrixGetActive(colorMatrix);
	}

	if (path && path->tileCount > 1) {
		pathWaterToTextDump(path, textDumps[2]);
		pathDistanceToTextDump(path, textDumps[0]);
		pathTimeToTextDump(path, textDumps[1]);
		DynamicArray_Add(narciaMap->paths, path);
	}
}

void Demo_InitAll(void) {

	AbsolutePos_t mainContent = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)};

	AbsolutePos_t textDump = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0.06, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)};

	AbsolutePos_t textDumpHeader = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.06, CONFIG_INIT_WINDOW_HEIGTH)};

	AbsolutePos_t mainHeader = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH)};

	AbsolutePos_t tileSearchBar = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0.80, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.5, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.65, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.95, CONFIG_INIT_WINDOW_HEIGTH)};

	container_t *mainContentContainer = windowAddContainer(mainContent);
	container_t *textDumpHeaderContainer = windowAddContainer(textDumpHeader);
	container_t *mainHeaderContainer = windowAddContainer(mainHeader);
	container_t *tileSearchBarContainer = windowAddContainer(tileSearchBar);

	mainContentContainer->fixed = true;
	mainContentContainer->layout.right = LAYOUT_BORDER_RIGHT;
	mainContentContainer->layout.bottom = LAYOUT_BORDER_BOTTOM;
	mainContentContainer->layout.offsetRight = -400;

	narciaMap = initNarciaMap((CommonPos_t){0, 0, 1, 1});
	containerAddWidget(mainContentContainer, (BaseWidget_t *)narciaMap);

	textDumpHeaderContainer->fixed = true;
	textDumpHeaderContainer->layout.right = LAYOUT_BORDER_RIGHT;
	textDumpHeaderContainer->layout.left = LAYOUT_BORDER_RIGHT;
	textDumpHeaderContainer->layout.offsetLeft = -400;

	mainHeaderContainer->fixed = true;
	mainHeaderContainer->layout.right = LAYOUT_BORDER_RIGHT;
	mainHeaderContainer->layout.offsetRight = -400;

	tileSearchBarContainer->fixed = true;
	tileSearchBarContainer->layout.right = LAYOUT_BORDER_RIGHT;
	tileSearchBarContainer->layout.left = LAYOUT_BORDER_RIGHT;
	tileSearchBarContainer->layout.top = LAYOUT_BORDER_BOTTOM;
	tileSearchBarContainer->layout.bottom = LAYOUT_BORDER_BOTTOM;
	tileSearchBarContainer->layout.offsetRight = -425;
	tileSearchBarContainer->layout.offsetLeft = -650;
	tileSearchBarContainer->layout.offsetBottom = -30;
	tileSearchBarContainer->layout.offsetTop = -125;

	textDumpMenuGroup = initSubmenuGroup();

	Drawable_t *textDumpBG = drawable_init9SliceImgRectangle((CommonPos_t){0, 0, 1, 1}, IDI_9_SLICE_2_TOP_LEFT, IDI_9_SLICE_2_TOP, IDI_9_SLICE_2_TOP_RIGHT, IDI_9_SLICE_2_LEFT, IDI_9_SLICE_2_CENTER, IDI_9_SLICE_2_RIGHT, IDI_9_SLICE_2_BOTTOM_LEFT, IDI_9_SLICE_2_BOTTOM, IDI_9_SLICE_2_BOTTOM_RIGHT, 60);

	for (int i = 0; i < 3; i++) {

		textDumpMenuButtons[i] = (buttonWidget_t *)customButton_initButton((CommonPos_t){0, i / 3.0f, (i + 1) / 3.0f, 1}, NULL, 1);
		containerAddWidget(textDumpHeaderContainer, (BaseWidget_t *)textDumpMenuButtons[i]);
		customButton_setTo3SliceBackground(textDumpMenuButtons[i], IDI_LEFT_BTN_2, IDI_CENTER_BTN_2, IDI_RIGHT_BTN_2);

		textDumpSubmenus[i] = MenuUi_SubmenuInit(textDumpMenuButtonNames[i], textDumpMenuButtons[i], textDumpMenuGroup);
		textDumpContainers[i] = MenuUi_SubmenuAddContainer(textDumpSubmenus[i], textDump);

		textDumpContainers[i]->fixed = true;
		textDumpContainers[i]->layout.right = LAYOUT_BORDER_RIGHT;
		textDumpContainers[i]->layout.left = LAYOUT_BORDER_RIGHT;
		textDumpContainers[i]->layout.bottom = LAYOUT_BORDER_BOTTOM;
		textDumpContainers[i]->layout.offsetLeft = -400;

		textDumps[i] = (textDumpWidget_t *)customTextDump_initTextDump((CommonPos_t){0.02, 0.05, 0.95, 0.98});
		containerAddWidget(textDumpContainers[i], (BaseWidget_t *)textDumps[i]);
		containerAddDrawable(textDumpContainers[i], textDumpBG);
		customTextDump_AddLine(textDumps[i], textDumpDefaultLines[i]);
		customTextDump_AddLine(textDumps[i], "---------------------------------------");
		customTextDump_AddLine(textDumps[i], "");
	}

	MenuUi_SubmenuAddLoadHandler(&textDumpDistanceMenuCallback, textDumpSubmenus[0]);
	MenuUi_SubmenuAddLoadHandler(&textDumpTimeMenuCallback, textDumpSubmenus[1]);
	MenuUi_SubmenuAddLoadHandler(&textDumpWaterMenuCallback, textDumpSubmenus[2]);

	inputX = customInput_initInput((CommonPos_t){0.2, 0.05, 0.35, 0.8});

	strncpy(inputX->defaultText, "X", 2);

	inputY = customInput_initInput((CommonPos_t){0.2, 0.4, 0.7, 0.8});

	strncpy(inputY->defaultText, "Y", 2);

	buttonWidget_t *goButton = customButton_initButton((CommonPos_t){0.2, 0.75, 0.95, 0.8}, &goButtonCallback, 0);
	customButton_setToIcon(goButton, IDI_GOBUTTON);

	Drawable_t *tileSearchBarBg = drawable_init9SliceImgRectangle((CommonPos_t){0, 0, 1, 1}, IDI_9_SLICE_1_TOP_LEFT, IDI_9_SLICE_1_TOP, IDI_9_SLICE_1_TOP_RIGHT, IDI_9_SLICE_1_LEFT, IDI_9_SLICE_1_CENTER, IDI_9_SLICE_1_RIGHT, IDI_9_SLICE_1_BOTTOM_LEFT, IDI_9_SLICE_1_BOTTOM, IDI_9_SLICE_1_BOTTOM_RIGHT, 30);
	Drawable_t *tileSearchBarLabelBg = drawable_init3SliceImgRectange((CommonPos_t){0.2, 0.77, 0.84, 0.9}, IDI_TEXT_BG_LEFT, IDI_TEXT_BG_CENTER, IDI_TEXT_BG_RIGHT);
	Drawable_t *tileSearchBarLabel = drawable_initLabel((CommonPos_t){0, 0, 0, 0}, tileSearchLabelText, &currentWindowState.activeTheme.label);

	containerAddDrawable(tileSearchBarContainer, tileSearchBarBg);
	containerAddDrawableAbsolute(tileSearchBarContainer, tileSearchBarLabelBg, (AbsolutePos_t){5, 25, 200, 35});
	containerAddDrawableAbsolute(tileSearchBarContainer, tileSearchBarLabel, (AbsolutePos_t){5, 65, 160, 35});
	containerAddWidgetAbsolute(tileSearchBarContainer, (BaseWidget_t *)inputX, (AbsolutePos_t){50, 0.05 * 225, 0.35 * 225, 80});
	containerAddWidgetAbsolute(tileSearchBarContainer, (BaseWidget_t *)inputY, (AbsolutePos_t){50, 0.4 * 225, 0.7 * 225, 80});
	containerAddWidgetAbsolute(tileSearchBarContainer, (BaseWidget_t *)goButton, (AbsolutePos_t){45, 170, 210, 85});

	colorMatrix = initColorMatrix((CommonPos_t){0.2, 0.85, 0.98, 0.9}, 4, 3);

	Drawable_t *mainHeaderBg = drawable_init9SliceImgRectangle((CommonPos_t){0, 0, 1, 1}, IDI_9_SLICE_1_TOP_LEFT, IDI_9_SLICE_1_TOP, IDI_9_SLICE_1_TOP_RIGHT, IDI_9_SLICE_1_LEFT, IDI_9_SLICE_1_CENTER, IDI_9_SLICE_1_RIGHT, IDI_9_SLICE_1_BOTTOM_LEFT, IDI_9_SLICE_1_BOTTOM, IDI_9_SLICE_1_BOTTOM_RIGHT, 50);
	containerAddDrawable(mainHeaderContainer, mainHeaderBg);

	Drawable_t *colorMatrixLabel = drawable_initLabel((CommonPos_t){0.2, 0.77, 0.84, 0.9}, colorMatrixText, &currentWindowState.activeTheme.label);
	Drawable_t *colorMatrixBg = drawable_init3SliceImgRectange((CommonPos_t){0.2, 0.77, 0.84, 0.9}, IDI_TEXT_BG_LEFT, IDI_TEXT_BG_CENTER, IDI_TEXT_BG_RIGHT);

	containerAddWidgetAbsolute(mainHeaderContainer, (BaseWidget_t *)colorMatrix, (AbsolutePos_t){0.2 * 100, 0.85 * 1400, 0.98 * 1400, 0.9 * 100});
	containerAddDrawableAbsolute(mainHeaderContainer, colorMatrixBg, (AbsolutePos_t){0.4 * 100, 0.75 * 1400, 0.845 * 1400, 0.7 * 100});
	containerAddDrawableAbsolute(mainHeaderContainer, colorMatrixLabel, (AbsolutePos_t){0.4 * 100, 0.765 * 1400, 0.835 * 1400, 0.7 * 100});

	buttonWidget_t *generatePathButton = customButton_initButton((CommonPos_t){0.3, 0.02, 0.12, 0.7}, &generatePath, 0);
	containerAddWidgetAbsolute(mainHeaderContainer, (BaseWidget_t *)generatePathButton, (AbsolutePos_t){0.3 * 100, 0.02 * 1400, 0.12 * 1400, 0.7 * 100});
	customButton_setButtonText(generatePathButton, generatePathText);
	customButton_setTo3SliceBackground(generatePathButton, IDI_LEFT_BTN, IDI_CENTER_BTN, IDI_RIGHT_BTN);

	Drawable_t *seperatorLine1 = drawable_initLine((CommonPos_t){0.2, 0.14, 0.14, 0.8}, &currentWindowState.activeTheme.line);
	containerAddDrawableAbsolute(mainHeaderContainer, seperatorLine1, (AbsolutePos_t){0.2 * 100, 0.14 * 1400, 0.14 * 1400, 0.8 * 100});

	buttonWidget_t *previousPathButton = customButton_initButton((CommonPos_t){0.2, 0.16, 0.27, 0.55}, &pathSelectButtonCallback, PREV_PATH_ID);
	containerAddWidgetAbsolute(mainHeaderContainer, (BaseWidget_t *)previousPathButton, (AbsolutePos_t){0.2 * 100, 0.16 * 1400, 0.27 * 1400, 0.55 * 100});
	customButton_setButtonText(previousPathButton, previousPathText);
	customButton_setTo3SliceBackground(previousPathButton, IDI_LEFT_BTN, IDI_CENTER_BTN, IDI_RIGHT_BTN);

	buttonWidget_t *nexPathButton = customButton_initButton((CommonPos_t){0.2, 0.29, 0.40, 0.55}, &pathSelectButtonCallback, NEXT_PATH_ID);
	containerAddWidgetAbsolute(mainHeaderContainer, (BaseWidget_t *)nexPathButton, (AbsolutePos_t){0.2 * 100, 0.29 * 1400, 0.40 * 1400, 0.55 * 100});
	customButton_setButtonText(nexPathButton, nextPathText);
	customButton_setTo3SliceBackground(nexPathButton, IDI_LEFT_BTN, IDI_CENTER_BTN, IDI_RIGHT_BTN);

	buttonWidget_t *deleteButton = customButton_initButton((CommonPos_t){0.65, 0.22, 0.34, 0.95}, &deletePathCallback, 0);
	containerAddWidgetAbsolute(mainHeaderContainer, (BaseWidget_t *)deleteButton, (AbsolutePos_t){0.65 * 100, 0.22 * 1400, 0.34 * 1400, 0.95 * 100});
	customButton_setButtonText(deleteButton, deleteText);
	customButton_setTo3SliceBackground(deleteButton, IDI_LEFT_BTN, IDI_CENTER_BTN, IDI_RIGHT_BTN);

	Drawable_t *seperatorLine2 = drawable_initLine((CommonPos_t){0.2, 0.42, 0.42, 0.8}, &currentWindowState.activeTheme.line);
	containerAddDrawableAbsolute(mainHeaderContainer, seperatorLine2, (AbsolutePos_t){0.2 * 100, 0.42 * 1400, 0.42 * 1400, 0.8 * 100});

	buttonWidget_t *clearSelectionButton = customButton_initButton((CommonPos_t){0.3, 0.44, 0.54, 0.7}, &clearSelection, 0);
	containerAddWidgetAbsolute(mainHeaderContainer, (BaseWidget_t *)clearSelectionButton, (AbsolutePos_t){0.3 * 100, 0.44 * 1400, 0.54 * 1400, 0.7 * 100});
	customButton_setButtonText(clearSelectionButton, clearSelectionText);
	customButton_setTo3SliceBackground(clearSelectionButton, IDI_LEFT_BTN, IDI_CENTER_BTN, IDI_RIGHT_BTN);

	Drawable_t *seperatorLine3 = drawable_initLine((CommonPos_t){0.2, 0.56, 0.56, 0.8}, &currentWindowState.activeTheme.line);
	containerAddDrawableAbsolute(mainHeaderContainer, seperatorLine3, (AbsolutePos_t){0.2 * 100, 0.56 * 1400, 0.56 * 1400, 0.8 * 100});

	buttonWidget_t *addEnemyHomebaseButton = customButton_initButton((CommonPos_t){0.2, 0.58, 0.73, 0.53}, NULL, 0);
	containerAddWidgetAbsolute(mainHeaderContainer, (BaseWidget_t *)addEnemyHomebaseButton, (AbsolutePos_t){0.2 * 100, 0.58 * 1400, 0.73 * 1400, 0.53 * 100});
	customButton_setButtonText(addEnemyHomebaseButton, generateEnemeyTownText);
	customButton_setTo3SliceBackground(addEnemyHomebaseButton, IDI_LEFT_BTN, IDI_CENTER_BTN, IDI_RIGHT_BTN);

	buttonWidget_t *addHomebaseButton = customButton_initButton((CommonPos_t){0.57, 0.58, 0.73, 0.9}, NULL, 0);
	containerAddWidgetAbsolute(mainHeaderContainer, (BaseWidget_t *)addHomebaseButton, (AbsolutePos_t){0.57 * 100, 0.58 * 1400, 0.73 * 1400, 0.9 * 100});
	customButton_setButtonText(addHomebaseButton, generateFriendlyTownText);
	customButton_setTo3SliceBackground(addHomebaseButton, IDI_LEFT_BTN, IDI_CENTER_BTN, IDI_RIGHT_BTN);

	Drawable_t *seperatorLine4 = drawable_initLine((CommonPos_t){0.2, 0.75, 0.75, 0.8}, &currentWindowState.activeTheme.line);
	containerAddDrawableAbsolute(mainHeaderContainer, seperatorLine4, (AbsolutePos_t){0.2 * 100, 0.745 * 1400, 0.745 * 1400, 0.8 * 100});

	mainHeaderContainer->fixedWidgets = true;
}
