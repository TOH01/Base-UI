#include <windows.h>
#include "common.h"
#include "MenuUi.h"
#include "UiUtils.h"
#include "container.h"
#include "costumButton.h"
#include <stdio.h>


#define MENU_UI_SUBMENU_MAX 10
#define MENU_UI_SUBMENU_START_ID 15100

#define MENU_UI_SUBMENU_GET_IDX(Id) (Id - MENU_UI_SUBMENU_START_ID)

int MenuUi_currentSubmenuIdx = 0;

int MenuUi_SubmenuIdCounter = MENU_UI_SUBMENU_START_ID;
MenuUi_Submenu_t submenus[MENU_UI_SUBMENU_MAX];

container_t * sidebarContainer = NULL;

const CommonPos_t sidebarPos = {UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(15), UI_UTILS_PERCENT(100)};
CommonPos_t sidebarButtonPos = {UI_UTILS_PERCENT(10), UI_UTILS_PERCENT(5), UI_UTILS_PERCENT(90), UI_UTILS_PERCENT(15)};

container_t * MenuUi_SubmenuAddContainer(int MenuId, containerPos_t pos){
    MenuUi_Submenu_t * submenu = &submenus[MENU_UI_SUBMENU_GET_IDX(MenuId)];
    
    container_t * container = initContainer(pos, submenu->WmParamHashTable);

    submenu->containers[submenu->containerIdx] = container;

    submenu->containerIdx++;

    return container;
}


void MenuUi_SubmenuSwap(int menuId) {

    // when swapping submenu reset active input
    activeInput = NULL;

    if (MENU_UI_SUBMENU_GET_IDX(menuId) == MenuUi_currentSubmenuIdx){
        return;
    }

    MenuUi_SubmenuCommandHandler(currentWindowState.hwnd, MENU_UI_SUBMENU_DESTROY_ID, 0, 0);

    MenuUi_currentSubmenuIdx = MENU_UI_SUBMENU_GET_IDX(menuId);
 
    MenuUi_SubmenuCommandHandler(currentWindowState.hwnd, MENU_UI_SUBMENU_LOAD_ID, 0, 0);

    topContainer = NULL;

    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

MenuUi_Submenu_t * getGurrentSubmenu(void){
    return &(submenus[MenuUi_currentSubmenuIdx]);
}

int MenuUi_SubmenuInit(char name[MENU_UI_MAX_NAME_LENGTH]){
    
    if ((MenuUi_SubmenuIdCounter - MENU_UI_SUBMENU_START_ID ) >= MENU_UI_SUBMENU_MAX){
        return -1;
    }

    if (sidebarContainer == NULL){
        sidebarContainer = windowAddContainer(sidebarPos);
    }

    MenuUi_Submenu_t * submenu = (MenuUi_Submenu_t *) calloc(1, sizeof(MenuUi_Submenu_t));
    
    submenu->SubmenuID = MenuUi_SubmenuIdCounter;
    MenuUi_SubmenuIdCounter++;
    
    submenu->WmParamHashTable = WmParamHandlerTable_Init();

    submenus[MENU_UI_SUBMENU_GET_IDX(submenu->SubmenuID)] = *submenu;

    memcpy(submenu->name, name, MENU_UI_MAX_NAME_LENGTH);


    buttonWidget_t * sidebarButton = initButton(sidebarButtonPos, &MenuUi_SubmenuSwap , submenu->SubmenuID);
    buttonSetText(sidebarButton, name);

    containerAddWidget(sidebarContainer, (BaseWidget_t *) sidebarButton);
    sidebarButtonPos.spacingTop += 0.1f;
    sidebarButtonPos.height += 0.1f;

    return submenu->SubmenuID;
}

void MenuUi_SubmenuCommandHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    if (submenus[MenuUi_currentSubmenuIdx].WmParamHashTable != NULL){
        
        if (WmParamHandlerTable_IdHasHandler(submenus[MenuUi_currentSubmenuIdx].WmParamHashTable, msg)){
            WmParamHandlerTable_CallHandlersOfId(submenus[MenuUi_currentSubmenuIdx].WmParamHashTable, hwnd, msg, wParam, lParam);
        }
    }
}

void MenuUi_SubmenuAddHandler(MessageHandler_t handler, int WmParamKey, int MenuId){
    WmParamHanderTable_Insert(submenus[MENU_UI_SUBMENU_GET_IDX(MenuId)].WmParamHashTable, WmParamKey, handler);
}


void MenuUi_SubmenuAddLoadHandler(MessageHandler_t handler, int id){
    WmParamHanderTable_Insert(submenus[MENU_UI_SUBMENU_GET_IDX(id)].WmParamHashTable, MENU_UI_SUBMENU_LOAD_ID, handler);
}

void MenuUi_SubmenuAddDestroyHandler(MessageHandler_t handler, int id){
    WmParamHanderTable_Insert(submenus[MENU_UI_SUBMENU_GET_IDX(id)].WmParamHashTable, MENU_UI_SUBMENU_DESTROY_ID, handler);
}

LRESULT MenuUi_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    // make sure that base submenu is loaded, when creating window

    MenuUi_SubmenuCommandHandler(hwnd, MENU_UI_SUBMENU_LOAD_ID, 0, 0);

    InvalidateRect(hwnd, NULL, FALSE);
}

LRESULT MenuUi_WmSizeHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    
    InvalidateRect(hwnd, NULL, FALSE);

    if (LOWORD(lParam) > CONFIG_MIN_WINDOW_WIDTH)
    {
        currentWindowState.currentWidth = LOWORD(lParam);;
    }
    if (HIWORD(lParam) > CONFIG_MIN_WINDOW_Height)
    {
        currentWindowState.currentHeight = HIWORD(lParam);
    }

}


void MenuUi_InitBaseHandlers(void){
    WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_CREATE, &MenuUi_WmCreateHook);
    WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_SIZE, &MenuUi_WmSizeHook);
}