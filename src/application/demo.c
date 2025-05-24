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
#include "colorMatrix.h"
#include "naricaMap.h"

buttonWidget_t *textDumpMenuButtons[3];
const char *textDumpMenuButtonNames[] = {
    "Distance",
    "Time",
    "Water",
};

const char colorMatrixText[] = "Path Color:";
const char generatePathText[] = "Generate Path";
const char previousPathText[] = "< Previous Path";
const char nextPathText[] = "Next Path >";
const char clearSelectionText[] = "Clear Selection";
const char generateFriendlyTownText[] = "Homebase +";
const char generateEnemeyTownText[] = "Enemy Homebase +";

container_t *textDumpContainers[3];
textDumpWidget_t *textDumps[3];

submenuGroup_t *textDumpMenuGroup;
MenuUi_Submenu_t *textDumpSubmenus[3];

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

	narciaMap_t * narciaMap = initNarciaMap((CommonPos_t){0, 0, 1, 1});
	containerAddWidget(mainContentContainer, (BaseWidget_t *) narciaMap);

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

	textDumpMenuGroup = initSubmenuGroup();

	for (int i = 0; i < 3; i++) {

		textDumpMenuButtons[i] = (buttonWidget_t *)customButton_initButton((CommonPos_t){0, i / 3.0f, (i + 1) / 3.0f, 1}, NULL, 1);
		containerAddWidget(textDumpHeaderContainer, (BaseWidget_t *)textDumpMenuButtons[i]);

		textDumpSubmenus[i] = MenuUi_SubmenuInit(textDumpMenuButtonNames[i], textDumpMenuButtons[i], textDumpMenuGroup);
		textDumpContainers[i] = MenuUi_SubmenuAddContainer(textDumpSubmenus[i], textDump);

		textDumpContainers[i]->fixed = true;
		textDumpContainers[i]->layout.right = LAYOUT_BORDER_RIGHT;
		textDumpContainers[i]->layout.left = LAYOUT_BORDER_RIGHT;
		textDumpContainers[i]->layout.bottom = LAYOUT_BORDER_BOTTOM;
		textDumpContainers[i]->layout.offsetLeft = -400;

		textDumps[i] = (textDumpWidget_t *)customTextDump_initTextDump((CommonPos_t){0, 0.01, 0.99, 0.99});
		containerAddWidget(textDumpContainers[i], (BaseWidget_t *)textDumps[i]);
		customTextDump_AddLine(textDumps[i], textDumpMenuButtonNames[i]);
	}

	inputWidget_t * inputX = customInput_initInput((CommonPos_t){0.2, 0.05, 0.35, 0.8});
	inputWidget_t * inputY = customInput_initInput((CommonPos_t){0.2, 0.4, 0.7, 0.8});
	buttonWidget_t * goButton = customButton_initButton((CommonPos_t){0.2, 0.75, 0.95, 0.8}, NULL, 0);

	containerAddWidget(tileSearchBarContainer, (BaseWidget_t *) inputX);
	containerAddWidget(tileSearchBarContainer, (BaseWidget_t *) inputY);
	containerAddWidget(tileSearchBarContainer, (BaseWidget_t *) goButton);

	colorMatrix_t * colorMatrix = initColorMatrix((CommonPos_t){0.2, 0.85, 0.98, 0.9}, 4, 3);

	Drawable_t * colorMatrixLabel = drawable_initLabel((CommonPos_t){0.2, 0.77, 0.84, 0.9}, colorMatrixText, &currentWindowState.activeTheme.label);

	containerAddWidget(mainHeaderContainer, (BaseWidget_t *) colorMatrix);
	containerAddDrawable(mainHeaderContainer, colorMatrixLabel);


	buttonWidget_t * generatePathButton = customButton_initButton((CommonPos_t){0.3, 0.02, 0.12, 0.7}, NULL, 0);
	containerAddWidget(mainHeaderContainer, (BaseWidget_t *) generatePathButton);
	customButton_setButtonText(generatePathButton, generatePathText);

	Drawable_t * seperatorLine1 = drawable_initLine((CommonPos_t){0.2, 0.14, 0.14, 0.8}, &currentWindowState.activeTheme.line);
	containerAddDrawable(mainHeaderContainer, seperatorLine1);


	buttonWidget_t * previousPathButton = customButton_initButton((CommonPos_t){0.3, 0.16, 0.27, 0.7}, NULL, 0);
	containerAddWidget(mainHeaderContainer, (BaseWidget_t *) previousPathButton);
	customButton_setButtonText(previousPathButton, previousPathText);
	
	buttonWidget_t * nexPathButton = customButton_initButton((CommonPos_t){0.3, 0.29, 0.40, 0.7}, NULL, 0);
	containerAddWidget(mainHeaderContainer, (BaseWidget_t *) nexPathButton);
	customButton_setButtonText(nexPathButton, nextPathText);

	Drawable_t * seperatorLine2 = drawable_initLine((CommonPos_t){0.2, 0.42, 0.42, 0.8}, &currentWindowState.activeTheme.line);
	containerAddDrawable(mainHeaderContainer, seperatorLine2);
	

	buttonWidget_t * clearSelectionButton = customButton_initButton((CommonPos_t){0.3, 0.44, 0.54, 0.7}, NULL, 0);
	containerAddWidget(mainHeaderContainer, (BaseWidget_t *) clearSelectionButton);
	customButton_setButtonText(clearSelectionButton, clearSelectionText);
	
	Drawable_t * seperatorLine3 = drawable_initLine((CommonPos_t){0.2, 0.56, 0.56, 0.8}, &currentWindowState.activeTheme.line);
	containerAddDrawable(mainHeaderContainer, seperatorLine3);


	buttonWidget_t * addEnemyHomebaseButton = customButton_initButton((CommonPos_t){0.2, 0.58, 0.73, 0.53}, NULL, 0);
	containerAddWidget(mainHeaderContainer, (BaseWidget_t *) addEnemyHomebaseButton);
	customButton_setButtonText(addEnemyHomebaseButton, generateEnemeyTownText);
	
	buttonWidget_t * addHomebaseButton = customButton_initButton((CommonPos_t){0.57, 0.58, 0.73, 0.9}, NULL, 0);
	containerAddWidget(mainHeaderContainer, (BaseWidget_t *) addHomebaseButton);
	customButton_setButtonText(addHomebaseButton, generateFriendlyTownText);

	Drawable_t * seperatorLine4 = drawable_initLine((CommonPos_t){0.2, 0.75, 0.75, 0.8}, &currentWindowState.activeTheme.line);
	containerAddDrawable(mainHeaderContainer, seperatorLine4);

	mainHeaderContainer->fixedWidgets = true;
}
