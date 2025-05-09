

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

MenuUi_Submenu_t * menu1;
MenuUi_Submenu_t * menu2;
submenuGroup_t * group1;

bool checkbox1value = 0;
int sliderValue = 1;

char label[30] = "Text Dump";

inputWidget_t *input;
textDumpWidget_t *textDump;

void button1Handler(int id) { printf("BUTTON %d CLICKED, Checkbox  Value : %d\n", id, checkbox1value); }

void button2Handler(int id) {
	customTextDump_AddLine(textDump, input->buffer);
	customInput_clearInput(input);
}

void magicButton(int id) {
	switch (sliderValue) {
	case 0:
		currentWindowState.activeTheme = OCEAN_BREEZE_THEME();
		break;
	case 1:
		currentWindowState.activeTheme = DARKMODE_THEME();
		break;
	case 2:
		currentWindowState.activeTheme = LIGHTMODE_THEME();
		break;
	default:
		break;
	}

	// since fonts will be overwritten need to reinitalize them
	UiTheme_initFonts();
}

void MenuUi_SubmenuInitAll(void) {

	char menu1_name[30] = "Menu 1";
	char menu2_name[30] = "Menu 2";

	CommonPos_t posMenu1Button = {UI_UTILS_PERCENT(1), UI_UTILS_PERCENT(1), UI_UTILS_PERCENT(6), UI_UTILS_PERCENT(4)};
	CommonPos_t posMenu2Button = {UI_UTILS_PERCENT(5), UI_UTILS_PERCENT(1), UI_UTILS_PERCENT(6), UI_UTILS_PERCENT(8)};

	buttonWidget_t *menu1Button = customButton_initButton(posMenu1Button, NULL, 0);
	buttonWidget_t *menu2Button = customButton_initButton(posMenu2Button, NULL, 0);

    rootContainerAddWidget((BaseWidget_t *) menu1Button);
    rootContainerAddWidget((BaseWidget_t *) menu2Button);

	group1 = initSubmenuGroup();

	menu1 = MenuUi_SubmenuInit(menu1_name, menu1Button, group1);
	menu2 = MenuUi_SubmenuInit(menu2_name, menu2Button, group1);

	CommonPos_t pos = {UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(15), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(50)};
	CommonPos_t pos2 = {UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(15), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(100)};
	CommonPos_t pos3 = {UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(100), UI_UTILS_PERCENT(100)};

	CommonPos_t pos4 = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(100), UI_UTILS_PERCENT(100)};

	CommonPos_t posHeaderTextDump = {UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(100), UI_UTILS_PERCENT(10)};

	Drawable_t *rectangle = drawable_initRectangle(posHeaderTextDump, &currentWindowState.activeTheme.rectangel);
	Drawable_t *text = drawable_initLabel(posHeaderTextDump, label, &currentWindowState.activeTheme.label);
	Drawable_t *line = drawable_initLine(posHeaderTextDump, &currentWindowState.activeTheme.line);

	container_t *container1 = MenuUi_SubmenuAddContainer(menu1, pos);
	container_t *container2 = MenuUi_SubmenuAddContainer(menu2, pos);
	container_t *textDumpContainer = windowAddContainer(pos3);

	textDumpContainer->movable = 1;

	textDump = customTextDump_initTextDump(pos4);

	container_t *sharedContainer = windowAddContainer(pos2);

	sharedContainer->movable = 1;
	sharedContainer->resizable = 1;

	containerAddWidget(textDumpContainer, (BaseWidget_t *)textDump);
	containerAddDrawable(textDumpContainer, rectangle);
	containerAddDrawable(textDumpContainer, line);
	containerAddDrawable(textDumpContainer, text);

	CommonPos_t posButton = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(25), UI_UTILS_PERCENT(25)};
	CommonPos_t posCheckbox = {UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(60), UI_UTILS_PERCENT(60)};
	CommonPos_t posSlider = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(20)};
	CommonPos_t posInput = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(90), UI_UTILS_PERCENT(25)};

	CommonPos_t posButton2 = {UI_UTILS_PERCENT(75), UI_UTILS_PERCENT(33), UI_UTILS_PERCENT(66), UI_UTILS_PERCENT(100)};
	CommonPos_t posSliderButton = {UI_UTILS_PERCENT(30), UI_UTILS_PERCENT(30), UI_UTILS_PERCENT(90), UI_UTILS_PERCENT(45)};

	buttonWidget_t *button = customButton_initButton(posButton, &button1Handler, 1);
	customButton_setButtonText(button, "Test");

	buttonWidget_t *button2 = customButton_initButton(posButton2, &button2Handler, 5);
	customButton_setButtonText(button2, "ADD");

	checkboxWidget_t *checkbox = customCheckbox_initCheckbox(posCheckbox, &checkbox1value);
	sliderWidget_t *slider = customSlider_initSlider(posSlider, &sliderValue, 3);
	buttonWidget_t *buttonSlider = customButton_initButton(posSliderButton, &magicButton, 69);

	customButton_setButtonText(buttonSlider, "Change Theme");

	input = customInput_initInput(posInput);

	containerAddWidget(container1, (BaseWidget_t *)slider);
	containerAddWidget(container1, (BaseWidget_t *)buttonSlider);
	containerAddWidget(container2, (BaseWidget_t *)button);
	containerAddWidget(container2, (BaseWidget_t *)checkbox);
	containerAddWidget(sharedContainer, (BaseWidget_t *)input);
	containerAddWidget(sharedContainer, (BaseWidget_t *)button2);
}
