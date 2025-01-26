#ifndef DISABLE_MENU

#include "MenuUi.h"
#include "common.h"
#include "widget.h"
#include <stdio.h>
#include "costumButton.h"
#include "costumCheckbox.h"

int menu1_key;
int menu2_key;

HWND button1 = NULL;
HWND button2 = NULL;

const button_t button1_struct = {
    .name = "Button 1",
    .wParam = 500,
    .pos = {
        .spacingTop = MENU_UI_BUTTON_SPACING_TOP,
        .height = MENU_UI_BUTTON_HEIGHT,
        .spacingLeft = MENU_UI_BUTTON_SPACING_LEFT + MENU_UI_MENU_WIDTH_PERCENTAGE,
        .width = MENU_UI_BUTTON_WIDTH,
    },
};

const button_t button2_struct = {
    .name = "Button 2",
    .wParam = 501,
    .pos = {
        .spacingTop = MENU_UI_BUTTON_SPACING_TOP,
        .height = MENU_UI_BUTTON_HEIGHT,
        .spacingLeft = MENU_UI_BUTTON_SPACING_LEFT + MENU_UI_MENU_WIDTH_PERCENTAGE,
        .width = MENU_UI_BUTTON_WIDTH,
    },
};

void InitialzeHandlers(void){
    MenuUi_InitBaseHandlers();
}

LRESULT Menu1_Load(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    button1 = UiUtils_initButton(hwnd, button1_struct);
}

LRESULT Menu2_Load(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    button2 = UiUtils_initButton(hwnd, button2_struct);
}

LRESULT Menu1_Destroy(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    DestroyWindow(button1);
    button1 = NULL;
}

LRESULT Menu2_Destroy(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    DestroyWindow(button2);
    button2 = NULL;
}

LRESULT Menu2_DestroyFake(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

}

LRESULT Menu1_WmCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(LOWORD(wParam)){
        case 500:
            printf("Menu 1 button 500 pressed\n");
    }
}

LRESULT Menu2_WmCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(LOWORD(wParam)){
         case 501:
            printf("Menu 2 button 501 pressed\n");
    }
}

bool checkbox1value = 0;

void button1Handler(void){
    printf("BUTTON 1 CLICKED, Checkbox 1 Value : %d\n", checkbox1value);
}

void MenuUi_SubmenuInitAll(void){
    
    char menu1_name[30] = "Menu 1";
    char menu2_name[30] = "Menu 2";
    
    menu1_key = MenuUi_SubmenuInit(menu1_name);
    menu2_key = MenuUi_SubmenuInit(menu2_name);

    MenuUi_SubmenuAddLoadHandler(&Menu1_Load, menu1_key);
    MenuUi_SubmenuAddLoadHandler(&Menu2_Load, menu2_key);
    MenuUi_SubmenuAddDestroyHandler(&Menu1_Destroy, menu1_key);
    MenuUi_SubmenuAddDestroyHandler(&Menu2_Destroy, menu2_key);
    MenuUi_SubmenuAddDestroyHandler(&Menu2_DestroyFake, menu2_key);

    MenuUi_SubmenuAddHandler(&Menu1_WmCommand, WM_COMMAND, menu1_key);
    MenuUi_SubmenuAddHandler(&Menu2_WmCommand, WM_COMMAND, menu2_key);

    CommonPos_t pos = {UI_UTILS_PERCENT(25), UI_UTILS_PERCENT(25), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(50)};

    container_t * container = MenuUi_SubmenuAddContainer(menu2_key, pos);
    
    CommonPos_t posButton = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(25), UI_UTILS_PERCENT(25)};
    CommonPos_t posCheckbox = {UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(50), UI_UTILS_PERCENT(60), UI_UTILS_PERCENT(60)};
    
    buttonWidget_t * button = initButton(posButton, &button1Handler);
    checkboxWidget_t * checkbox = initCheckbox(posCheckbox, &checkbox1value);

    containerAddWidget(container, (BaseWidget_t *) button);
    containerAddWidget(container, (BaseWidget_t *) checkbox);

}

#endif