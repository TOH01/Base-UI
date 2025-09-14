#include "common.h"
#include "container.h"
#include "customButton.h"

const char mainButtonText[] = "Add";

void Demo_InitAll(void) {

	AbsolutePos_t mainContent = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(1, CONFIG_INIT_WINDOW_HEIGTH)};
    AbsolutePos_t mainHeader = (AbsolutePos_t){UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_HEIGTH), UI_UTILS_CALCULATE_PERCENTAGE(0, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.7, CONFIG_INIT_WINDOW_WIDTH), UI_UTILS_CALCULATE_PERCENTAGE(0.1, CONFIG_INIT_WINDOW_HEIGTH)};

    container_t *mainContentContainer = windowAddContainer(mainContent);
    container_t *mainHeaderContainer = windowAddContainer(mainHeader);

    mainContentContainer->fixed = true;
	mainContentContainer->layout.right = LAYOUT_BORDER_RIGHT;
	mainContentContainer->layout.bottom = LAYOUT_BORDER_BOTTOM;

    mainHeaderContainer->fixed = true;
	mainHeaderContainer->layout.right = LAYOUT_BORDER_RIGHT;

    buttonWidget_t *mainButton = customButton_initButton((CommonPos_t){0.3, 0.85, 0.95, 0.7}, NULL, 0);
	containerAddWidgetAbsolute(mainHeaderContainer, (BaseWidget_t *)mainButton, (AbsolutePos_t){0.3 * 100, 0.5 * 1400, 0.6 * 1400, 0.7 * 100});
	customButton_setButtonText(mainButton, mainButtonText);

}