#ifndef DISABLE_MENU

#include "MenuUi.h"
#include "common.h"
#include "widget.h"
#include <stdio.h>
#include "customButton.h"
#include "costumCheckbox.h"
#include "costumSlider.h"
#include "costumInput.h"
#include "textDump.h"

int menu1_key;
int menu2_key;

bool checkbox1value = 0;
int sliderValue = 2;

inputWidget_t * input;
textDumpWidget_t * textDump;

void InitialzeHandlers(void){
    MenuUi_InitBaseHandlers();
}

void button1Handler(int id){
    printf("BUTTON %d CLICKED, Checkbox  Value : %d\n", id, checkbox1value);
}

void button2Handler(int id){
    costumTextDump_AddLine(textDump, input->buffer);
    costumInput_clearInput(input);
}

void MenuUi_SubmenuInitAll(void){
    
    char menu1_name[30] = "Menu 1";
    char menu2_name[30] = "Menu 2";
    
    menu1_key = MenuUi_SubmenuInit(menu1_name);
    menu2_key = MenuUi_SubmenuInit(menu2_name);

    CommonPos_t pos = {UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(15), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(50)};
    CommonPos_t pos2 = {UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(15), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(100)};
    CommonPos_t pos3 = {UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(100), UI_UTILS_PERCENT(100)};
    CommonPos_t pos4 = {UI_UTILS_PERCENT(1), UI_UTILS_PERCENT(1), UI_UTILS_PERCENT(99), UI_UTILS_PERCENT(99)};

    container_t * container1 = MenuUi_SubmenuAddContainer(menu1_key, pos);
    container_t * container2 = MenuUi_SubmenuAddContainer(menu2_key, pos);

    container_t * textDumpContainer = windowAddContainer(pos3);
    
    textDump = costumTextDump_initTextDump(pos4);

    container_t * sharedContainer = windowAddContainer(pos2);

    containerAddWidget(textDumpContainer, (BaseWidget_t *)textDump);
    
    CommonPos_t posButton = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(25), UI_UTILS_PERCENT(25)};
    CommonPos_t posCheckbox = {UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(60), UI_UTILS_PERCENT(60)};
    CommonPos_t posSlider = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(20)};
    CommonPos_t posInput = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(90), UI_UTILS_PERCENT(25)};
    
    CommonPos_t posButton2 = {UI_UTILS_PERCENT(33), UI_UTILS_PERCENT(33), UI_UTILS_PERCENT(66), UI_UTILS_PERCENT(45)};
    
    buttonWidget_t * button = customButton_initButton(posButton, &button1Handler, 1);
    customButton_SetButtonText(button, "Test");

    buttonWidget_t * button2 = customButton_initButton(posButton2, &button2Handler, 5);
    customButton_SetButtonText(button2, "ADD");

    checkboxWidget_t * checkbox = costumCheckbox_initCheckbox(posCheckbox, &checkbox1value);
    sliderWidget_t * slider = costumSlider_initSlider(posSlider, &sliderValue, 5);

    input = costumInput_initInput(posInput);

    containerAddWidget(container1, (BaseWidget_t *) slider);
    containerAddWidget(container2, (BaseWidget_t *) button);
    containerAddWidget(container2, (BaseWidget_t *) checkbox);
    containerAddWidget(sharedContainer, (BaseWidget_t *) input);
    containerAddWidget(sharedContainer, (BaseWidget_t*) button2);

}

#endif