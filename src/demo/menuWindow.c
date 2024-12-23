#ifndef DISABLE_MENU

#include "MenuUi.h"
#include "common.h"

LRESULT MenuUi_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MenuUi_RenderMenuButtons(hwnd);

    MenuUi_SubmenuSwap(hwnd, 0);

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

    MenuUi_ResizeMenuUi(currentWindowState.currentWidth,  currentWindowState.currentHeight);

    MenuUi_ResizeMenuContent(currentWindowState.currentWidth,  currentWindowState.currentHeight);

    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT MenuUi_WmPaintHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    currentWindowState.hdc = BeginPaint(hwnd, &currentWindowState.ps);
    
    MenuUi_DrawMenuUi(hwnd, currentWindowState.currentWidth,  currentWindowState.currentHeight);

    MenuUi_DrawMenuContent(hwnd, currentWindowState.currentWidth,  currentWindowState.currentHeight);

    EndPaint(hwnd, &currentWindowState.ps);
}

LRESULT MenuUi_WmCommandHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (MenuUi_IsSidebarCommand(LOWORD(wParam)))
    {
        MenuUi_SubmenuSwap(hwnd, LOWORD(wParam));
        SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(currentWindowState.currentWidth,  currentWindowState.currentHeight));
    }

    else
    {
        MenuUi_SubmenuWmParamCallback(hwnd, wParam);
    }
}

void InitializeHooks(){
    
    WmParamHashTable_Insert(currentWindowState.wmParamHashTable, WM_CREATE, &MenuUi_WmCreateHook);
    WmParamHashTable_Insert(currentWindowState.wmParamHashTable, WM_SIZE, &MenuUi_WmSizeHook);
    WmParamHashTable_Insert(currentWindowState.wmParamHashTable, WM_PAINT, &MenuUi_WmPaintHook);
    WmParamHashTable_Insert(currentWindowState.wmParamHashTable, WM_COMMAND, &MenuUi_WmCommandHook);
}

void MenuUi_SubmenuInitAll(void){

}

#endif