#include <windows.h>
#include "common.h"
#include "MenuUi.h"
#include "UiUtils.h"

#define MENU_UI_SUBMENU_MAX 10
#define MENU_UI_SUBMENU_START_ID 15100

#define MENU_UI_SUBMENU_GET_IDX(Id) (Id - MENU_UI_SUBMENU_START_ID)

int MenuUi_currentSubmenuIdx = 0;

int MenuUi_SubmenuIdCounter = MENU_UI_SUBMENU_START_ID;
MenuUi_Submenu_t submenus[MENU_UI_SUBMENU_MAX];

void MenuUi_SubmenuInit(char name[30], MenuUi_Submenu_Callback_t callbacks){
    
    if ((MenuUi_SubmenuIdCounter - MENU_UI_SUBMENU_START_ID ) >= MENU_UI_SUBMENU_MAX){
        return;
    }

    MenuUi_Submenu_t submenu;
    
    submenu.SubmenuCallbacks = callbacks;
    
    submenu.SubmenuID = MenuUi_SubmenuIdCounter;
    MenuUi_SubmenuIdCounter++;
    
    button_t MenuUi_SubmenuLoadButton = {
        .wParam = submenu.SubmenuID,
        .pos = {
            .spacingTop = MENU_UI_BUTTON_GET_START_Y((MENU_UI_SUBMENU_GET_IDX(submenu.SubmenuID))),
            .height = MENU_UI_BUTTON_HEIGHT,
            .spacingLeft = MENU_UI_BUTTON_SPACING_LEFT,
            .width = MENU_UI_BUTTON_WIDTH,
        },
    };

    memcpy(MenuUi_SubmenuLoadButton.name, name, 30);

    submenu.SubmenuLoadButton = MenuUi_SubmenuLoadButton;

    submenus[MENU_UI_SUBMENU_GET_IDX(submenu.SubmenuID)] = submenu;
}

void MenuUi_SubmenuWmParamCallback(HWND hwnd, WPARAM wParam){
    if (submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuWMParamCallback != NULL){
        (*submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuWMParamCallback)(hwnd, wParam);
    }
}

void MenuUi_SubmenuLoadContent(HWND hwnd){
    if (submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuRenderContent != NULL){
        (*submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuRenderContent)(hwnd);
    }
}

void MenuUi_SubmenuDestroyContent(void){
    if (submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuDestroyContent != NULL){
        (*submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuDestroyContent)();
    }
}

void MenuUi_SubmenuDrawContent(HWND hwnd, int width, int height){
    if (submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuDrawContent != NULL){
        (*submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuDrawContent)(hwnd, width, height);
    }
}

void MenuUi_SubmenuResizeContent(int width, int height){
        if (submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuResizeContent != NULL){
        (*submenus[MenuUi_currentSubmenuIdx].SubmenuCallbacks.SubmenuResizeContent)(width, height);
    }
}

void MenuUi_RenderMenuButtons(HWND hwnd){
            
        for (int i = 0; i < MENU_UI_SUBMENU_GET_IDX(MenuUi_SubmenuIdCounter); i++){
            if ((submenus[i]).SubmenuLoadButton.wParam != 0){
                submenus[i].hSubmenuLoadButton = UiUtils_initButton(hwnd, submenus[i].SubmenuLoadButton);
            }
        }

        MenuUi_SubmenuLoadContent(hwnd);

}

void MenuUi_DrawMenuUi(HWND hwnd, int width, int height){
    SelectObject(currentWindowState.hdc, currentWindowState.hPen);
    Rectangle(currentWindowState.hdc, 0, 0, UI_UTILS_CALCULATE_PERCENTAGE(width, MENU_UI_MENU_WIDTH_PERCENTAGE), height);
}

void MenuUi_SubmenuSwap(HWND hwnd, int menuId) {

    if (menuId == MenuUi_currentSubmenuIdx){
        return;
    }

    MenuUi_SubmenuDestroyContent();

    MenuUi_currentSubmenuIdx = MENU_UI_SUBMENU_GET_IDX(menuId);

    MenuUi_SubmenuLoadContent(hwnd);

    InvalidateRect(hwnd, NULL, TRUE);
}

bool MenuUi_IsSidebarCommand(WORD commandId){
    
    for (int i = 0; i < MENU_UI_SUBMENU_GET_IDX(MenuUi_SubmenuIdCounter); i++){
        if (commandId == submenus[i].SubmenuID){
            return true;
        }
    }

    return false;

}

void MenuUi_ResizeMenuUi(int width, int height){

    for (int i = 0; i < MENU_UI_SUBMENU_GET_IDX(MenuUi_SubmenuIdCounter); i++){
        UiUtils_moveButton(submenus[i].hSubmenuLoadButton, submenus[i].SubmenuLoadButton.pos, width, height);
    }

}

void MenuUi_ResizeMenuContent(int width, int height){
    MenuUi_SubmenuResizeContent(width, height);
}

void MenuUi_DrawMenuContent(HWND hwnd, int currentWidth, int currentHeight){
    MenuUi_SubmenuDrawContent(hwnd, currentWidth, currentHeight);
}
