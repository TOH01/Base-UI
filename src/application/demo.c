#include "common.h"
#include "customButton.h"
#include "customCheckbox.h"
#include "customInput.h"
#include "customSlider.h"
#include "drawable.h"
#include "menu.h"
#include "textDump.h"
#include "widget.h"
#include <stdio.h>

buttonWidget_t *textDumpMenuButtons[3];
const char *textDumpMenuButtonNames[] = {
    "Distance",
    "Time",
    "Water",
};

void Demo_InitAll(void) {

	AbsolutePos_t mainContent = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)};

	AbsolutePos_t textDump = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0.06, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)};

	AbsolutePos_t textDumpHeader = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.06, CONFIG_INIT_WINDOW_HEIGTH)};

	AbsolutePos_t mainHeader = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH)};

	AbsolutePos_t tileSearchBar = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0.80, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0.5, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.65, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.95, CONFIG_INIT_WINDOW_HEIGTH)};

	container_t *mainContentContainer = windowAddContainer(mainContent);
	container_t *textDumpContainer = windowAddContainer(textDump);
	container_t *textDumpHeaderContainer = windowAddContainer(textDumpHeader);
	container_t *mainHeaderContainer = windowAddContainer(mainHeader);
	container_t *tileSearchBarContainer = windowAddContainer(tileSearchBar);

	textDumpContainer->fixed = true;
	textDumpContainer->layout.right = LAYOUT_BORDER_RIGHT;
	textDumpContainer->layout.left = LAYOUT_BORDER_RIGHT;
	textDumpContainer->layout.bottom = LAYOUT_BORDER_BOTTOM;
	textDumpContainer->layout.offsetLeft = -400;

	mainContentContainer->fixed = true;
	mainContentContainer->layout.right = LAYOUT_BORDER_RIGHT;
	mainContentContainer->layout.bottom = LAYOUT_BORDER_BOTTOM;
	mainContentContainer->layout.offsetRight = -400;

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
	tileSearchBarContainer->layout.offsetTop = -90;

	for (int i = 0; i < 3; i++) {
		textDumpMenuButtons[i] = (buttonWidget_t *)customButton_initButton((CommonPos_t){0, i / 3.0f, (i + 1) / 3.0f, 1}, NULL, 1);
		containerAddWidget(textDumpHeaderContainer, (BaseWidget_t *)textDumpMenuButtons[i]);
		customButton_setButtonText(textDumpMenuButtons[i], textDumpMenuButtonNames[i]);
	}

	mainHeaderContainer->fixedWidgets = true;
}
