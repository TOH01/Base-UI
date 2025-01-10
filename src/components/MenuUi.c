#include <windows.h>
#include "common.h"
#include "MenuUi.h"
#include "UiUtils.h"
#include <stdio.h>

#define MENU_UI_SUBMENU_MAX 10
#define MENU_UI_SUBMENU_START_ID 15100

#define MENU_UI_SUBMENU_GET_IDX(Id) (Id - MENU_UI_SUBMENU_START_ID)

int MenuUi_currentSubmenuIdx = 0;

int MenuUi_SubmenuIdCounter = MENU_UI_SUBMENU_START_ID;
MenuUi_Submenu_t submenus[MENU_UI_SUBMENU_MAX];

container_t * MenuUi_SubmenuAddContainer(int MenuId, containerPos_t pos){
    MenuUi_Submenu_t * submenu = &submenus[MENU_UI_SUBMENU_GET_IDX(MenuId)];
    
    container_t * container = initContainer(pos, submenu->WmParamHashTable);

    submenu->containers[submenu->containerIdx] = container;

    submenu->containerIdx++;

    return container;
}

MenuUi_Submenu_t * getGurrentSubmenu(void){
    return &(submenus[MenuUi_currentSubmenuIdx]);
}

int MenuUi_SubmenuInit(char name[30]){
    
    if ((MenuUi_SubmenuIdCounter - MENU_UI_SUBMENU_START_ID ) >= MENU_UI_SUBMENU_MAX){
        return -1;
    }

    MenuUi_Submenu_t * submenu = (MenuUi_Submenu_t *) calloc(1, sizeof(MenuUi_Submenu_t));
    
    submenu->SubmenuID = MenuUi_SubmenuIdCounter;
    MenuUi_SubmenuIdCounter++;
    
    button_t MenuUi_SubmenuLoadButton = {
        .wParam = submenu->SubmenuID,
        .pos = {
            .spacingTop = MENU_UI_BUTTON_GET_START_Y((MENU_UI_SUBMENU_GET_IDX(submenu->SubmenuID))),
            .height = MENU_UI_BUTTON_HEIGHT,
            .spacingLeft = MENU_UI_BUTTON_SPACING_LEFT,
            .width = MENU_UI_BUTTON_WIDTH,
        },
    };

    submenu->WmParamHashTable = WmParamHandlerTable_Init();

    memcpy(MenuUi_SubmenuLoadButton.name, name, 30);

    submenu->SubmenuLoadButton = MenuUi_SubmenuLoadButton;

    submenus[MENU_UI_SUBMENU_GET_IDX(submenu->SubmenuID)] = *submenu;

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

void MenuUi_RenderMenuButtons(HWND hwnd){
            
        for (int i = 0; i < MENU_UI_SUBMENU_GET_IDX(MenuUi_SubmenuIdCounter); i++){
            if ((submenus[i]).SubmenuLoadButton.wParam != 0){
                submenus[i].hSubmenuLoadButton = UiUtils_initButton(hwnd, submenus[i].SubmenuLoadButton);
            }
        }
}

#include <stdio.h>

void MenuUi_SubmenuSwap(HWND hwnd, int menuId) {

    if (MENU_UI_SUBMENU_GET_IDX(menuId) == MenuUi_currentSubmenuIdx){
        return;
    }

    MenuUi_SubmenuCommandHandler(hwnd, MENU_UI_SUBMENU_DESTROY_ID, 0, 0);

    MenuUi_currentSubmenuIdx = MENU_UI_SUBMENU_GET_IDX(menuId);

    MenuUi_SubmenuCommandHandler(hwnd, MENU_UI_SUBMENU_LOAD_ID, 0, 0);

    InvalidateRect(hwnd, NULL, TRUE);
}

void MenuUi_SubmenuAddLoadHandler(MessageHandler_t handler, int id){
    WmParamHanderTable_Insert(submenus[MENU_UI_SUBMENU_GET_IDX(id)].WmParamHashTable, MENU_UI_SUBMENU_LOAD_ID, handler);
}

void MenuUi_SubmenuAddDestroyHandler(MessageHandler_t handler, int id){
    WmParamHanderTable_Insert(submenus[MENU_UI_SUBMENU_GET_IDX(id)].WmParamHashTable, MENU_UI_SUBMENU_DESTROY_ID, handler);
}

void MenuUi_ResizeSidebar(int width, int height){

    for (int i = 0; i < MENU_UI_SUBMENU_GET_IDX(MenuUi_SubmenuIdCounter); i++){
        UiUtils_moveButton(submenus[i].hSubmenuLoadButton, submenus[i].SubmenuLoadButton.pos, width, height);
    }

}

bool MenuUi_IsSidebarCommand(WORD commandId){
    
    for (int i = 0; i < MENU_UI_SUBMENU_GET_IDX(MenuUi_SubmenuIdCounter); i++){
        if (commandId == submenus[i].SubmenuID){
            return true;
        }
    }

    return false;

}

void MenuUi_DrawSidebar(HWND hwnd, int width, int height){
    SelectObject(currentWindowState.hdc, currentWindowState.hPen);
    Rectangle(currentWindowState.hdc, 0, 0, UI_UTILS_CALCULATE_PERCENTAGE(width, MENU_UI_MENU_WIDTH_PERCENTAGE), height);
}

LRESULT MenuUi_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MenuUi_RenderMenuButtons(hwnd);

    MenuUi_SubmenuSwap(hwnd, MENU_UI_SUBMENU_START_ID);

    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT MenuUi_WmSizeHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (LOWORD(lParam) > CONFIG_MIN_WINDOW_WIDTH)
    {
        currentWindowState.currentWidth = LOWORD(lParam);;
    }
    if (HIWORD(lParam) > CONFIG_MIN_WINDOW_Height)
    {
        currentWindowState.currentHeight = HIWORD(lParam);
    }

    MenuUi_ResizeSidebar(currentWindowState.currentWidth,  currentWindowState.currentHeight);

    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT MenuUi_WmPaintHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    currentWindowState.hdc = BeginPaint(hwnd, &currentWindowState.ps);
    
    MenuUi_DrawSidebar(hwnd, currentWindowState.currentWidth,  currentWindowState.currentHeight);

    EndPaint(hwnd, &currentWindowState.ps);
}

LRESULT MenuUi_WmCommandHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (MenuUi_IsSidebarCommand(LOWORD(wParam)))
    {
        MenuUi_SubmenuSwap(hwnd, LOWORD(wParam));
    }
}

void MenuUi_InitBaseHandlers(void){
    WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_CREATE, &MenuUi_WmCreateHook);
    WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_SIZE, &MenuUi_WmSizeHook);
    WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_PAINT, &MenuUi_WmPaintHook);
    WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_COMMAND, &MenuUi_WmCommandHook);
}