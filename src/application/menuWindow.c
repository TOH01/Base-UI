#ifndef DISABLE_MENU

#include "MenuUi.h"
#include "common.h"
#include "widget.h"
#include <stdio.h>
#include "costumButton.h"
#include "costumCheckbox.h"
#include "costumSlider.h"

int menu1_key;
int menu2_key;

bool checkbox1value = 0;
int sliderValue = 2;

void InitialzeHandlers(void){
    MenuUi_InitBaseHandlers();
}

void button1Handler(void){
    printf("BUTTON 1 CLICKED, Checkbox 1 Value : %d\n", checkbox1value);
}

void MenuUi_SubmenuInitAll(void){
    
    char menu1_name[30] = "Menu 1";
    char menu2_name[30] = "Menu 2";
    
    menu1_key = MenuUi_SubmenuInit(menu1_name);
    menu2_key = MenuUi_SubmenuInit(menu2_name);

    CommonPos_t pos = {UI_UTILS_PERCENT(25), UI_UTILS_PERCENT(25), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(50)};

    container_t * container1 = MenuUi_SubmenuAddContainer(menu1_key, pos);
    container_t * container2 = MenuUi_SubmenuAddContainer(menu2_key, pos);
    
    CommonPos_t posButton = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(25), UI_UTILS_PERCENT(25)};
    CommonPos_t posCheckbox = {UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(60), UI_UTILS_PERCENT(60)};
    CommonPos_t posSlider = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(20)};
    
    buttonWidget_t * button = initButton(posButton, &button1Handler);
    checkboxWidget_t * checkbox = initCheckbox(posCheckbox, &checkbox1value);
    sliderWidget_t * slider = initSlider(posSlider, &sliderValue, 5);

    containerAddWidget(container1, (BaseWidget_t *) slider);
    containerAddWidget(container2, (BaseWidget_t *) button);
    containerAddWidget(container2, (BaseWidget_t *) checkbox);

}

#endif