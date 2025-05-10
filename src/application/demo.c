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

MenuUi_Submenu_t *menu1;
MenuUi_Submenu_t *menu2;
submenuGroup_t *group1;

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

void Demo_InitAll(void) {
    submenuGroup_t *groupA = initSubmenuGroup();
    submenuGroup_t *groupB = initSubmenuGroup();

    // Menu toggle buttons
    buttonWidget_t *btnA1 = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(1), UI_UTILS_PERCENT(1),
        UI_UTILS_PERCENT(17), UI_UTILS_PERCENT(7)}, NULL, 0);


    buttonWidget_t *btnA2 = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(1), UI_UTILS_PERCENT(19),
        UI_UTILS_PERCENT(33), UI_UTILS_PERCENT(7)}, NULL, 0);


    buttonWidget_t *btnB1 = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(1), UI_UTILS_PERCENT(78),
        UI_UTILS_PERCENT(89), UI_UTILS_PERCENT(7)}, NULL, 0);


    buttonWidget_t *btnB2 = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(1), UI_UTILS_PERCENT(91),
        UI_UTILS_PERCENT(99), UI_UTILS_PERCENT(7)}, NULL, 0);


    rootContainerAddWidget((BaseWidget_t *)btnA1);
    rootContainerAddWidget((BaseWidget_t *)btnA2);
    rootContainerAddWidget((BaseWidget_t *)btnB1);
    rootContainerAddWidget((BaseWidget_t *)btnB2);

    // Submenus
    MenuUi_Submenu_t *menuA1 = MenuUi_SubmenuInit("User Widgets", btnA1, groupA);
    MenuUi_Submenu_t *menuA2 = MenuUi_SubmenuInit("Log Output", btnA2, groupA);
    MenuUi_Submenu_t *menuB1 = MenuUi_SubmenuInit("Themes", btnB1, groupB);
    MenuUi_Submenu_t *menuB2 = MenuUi_SubmenuInit("Font", btnB2, groupB);

    // Submenu containers
    container_t *contA1 = MenuUi_SubmenuAddContainer(menuA1, (CommonPos_t){
        UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(5),
        UI_UTILS_PERCENT(48), UI_UTILS_PERCENT(45)});
    container_t *contA2 = MenuUi_SubmenuAddContainer(menuA2, (CommonPos_t){
        UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(5),
        UI_UTILS_PERCENT(48), UI_UTILS_PERCENT(90)});
    container_t *contB1 = MenuUi_SubmenuAddContainer(menuB1, (CommonPos_t){
        UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(52),
        UI_UTILS_PERCENT(95), UI_UTILS_PERCENT(45)});
    container_t *contB2 = MenuUi_SubmenuAddContainer(menuB2, (CommonPos_t){
        UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(52),
        UI_UTILS_PERCENT(95), UI_UTILS_PERCENT(90)});

	contA1->movable = 1;
	contA2->movable = 1;
	contB2->movable = 1;
	contB1->movable = 1;
	contA1->resizable = 1;
	contA2->resizable = 1;
	contB1->resizable = 1;
	contB2->resizable = 1;

    // === Group A1: Checkbox + Action ===
    checkboxWidget_t *checkbox = customCheckbox_initCheckbox((CommonPos_t){
        UI_UTILS_PERCENT(55), UI_UTILS_PERCENT(45),
        UI_UTILS_PERCENT(55), UI_UTILS_PERCENT(65)}, &checkbox1value);

    buttonWidget_t *printBtn = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(30),
        UI_UTILS_PERCENT(70), UI_UTILS_PERCENT(30)}, &button1Handler, 1);
    customButton_setButtonText(printBtn, "Print Checkbox");

    containerAddWidget(contA1, (BaseWidget_t *)checkbox);
    containerAddWidget(contA1, (BaseWidget_t *)printBtn);

    // === Group A2: Input + TextDump ===
    input = customInput_initInput((CommonPos_t){
        UI_UTILS_PERCENT(15), UI_UTILS_PERCENT(5),
        UI_UTILS_PERCENT(95), UI_UTILS_PERCENT(30)});

    buttonWidget_t *addBtn = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(35), UI_UTILS_PERCENT(5),
        UI_UTILS_PERCENT(95), UI_UTILS_PERCENT(50)}, &button2Handler, 5);
    customButton_setButtonText(addBtn, "Add Log Line");

    textDump = customTextDump_initTextDump((CommonPos_t){
        UI_UTILS_PERCENT(55), UI_UTILS_PERCENT(5),
        UI_UTILS_PERCENT(95), UI_UTILS_PERCENT(100)});

    containerAddWidget(contA2, (BaseWidget_t *)input);
    containerAddWidget(contA2, (BaseWidget_t *)addBtn);
    containerAddWidget(contA2, (BaseWidget_t *)textDump);

    containerAddDrawable(contA2, drawable_initLabel((CommonPos_t){
        UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(5),
        UI_UTILS_PERCENT(95), UI_UTILS_PERCENT(15)}, label, &currentWindowState.activeTheme.label));

    // === Group B1: Theme Selection ===
    sliderWidget_t *themeSlider = customSlider_initSlider((CommonPos_t){
        UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(30),
        UI_UTILS_PERCENT(70), UI_UTILS_PERCENT(25)}, &sliderValue, 3);

    buttonWidget_t *applyThemeBtn = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(30), UI_UTILS_PERCENT(10),
        UI_UTILS_PERCENT(90), UI_UTILS_PERCENT(50)}, &magicButton, 69);
    customButton_setButtonText(applyThemeBtn, "Apply Selected Theme");

    containerAddWidget(contB1, (BaseWidget_t *)themeSlider);
    containerAddWidget(contB1, (BaseWidget_t *)applyThemeBtn);

    // === Group B2: Font Settings ===
    buttonWidget_t *fontSmallBtn = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10),
        UI_UTILS_PERCENT(45), UI_UTILS_PERCENT(25)}, NULL, 10);
    customButton_setButtonText(fontSmallBtn, "Small Font");

    buttonWidget_t *fontLargeBtn = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(55),
        UI_UTILS_PERCENT(90), UI_UTILS_PERCENT(25)}, NULL, 11);
    customButton_setButtonText(fontLargeBtn, "Large Font");

    buttonWidget_t *resetBtn = customButton_initButton((CommonPos_t){
        UI_UTILS_PERCENT(40), UI_UTILS_PERCENT(10),
        UI_UTILS_PERCENT(90), UI_UTILS_PERCENT(60)}, NULL, 99);
    customButton_setButtonText(resetBtn, "Reset UI Layout");

    containerAddWidget(contB2, (BaseWidget_t *)fontSmallBtn);
    containerAddWidget(contB2, (BaseWidget_t *)fontLargeBtn);
    containerAddWidget(contB2, (BaseWidget_t *)resetBtn);
}

