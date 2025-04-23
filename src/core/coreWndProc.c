#include "Common.h"
#include "coreWndProc.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"
#include <stdio.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        WmParamHandlerTable_Destroy(currentWindowState.handlerTable);
        PostQuitMessage(0);
        break;
    case WM_PAINT:
    
    int mapMode = GetMapMode(currentWindowState.hdc);

        currentWindowState.hdc = BeginPaint(hwnd, &currentWindowState.ps);

        // Create and select the memory device context
        currentWindowState.memHDC = CreateCompatibleDC(currentWindowState.hdc);
        currentWindowState.memBitmap = CreateCompatibleBitmap(currentWindowState.hdc, currentWindowState.width, currentWindowState.height);
        HBITMAP hbmOld = SelectObject(currentWindowState.memHDC, currentWindowState.memBitmap);

        // call draw handlers (draw to memDC)
        #ifndef DISABLE_MENU
        WmParamHandlerTable_CallHandlersOfId(MenuUi_GetCurrentSubmenu()->WmParamHashTable, hwnd, msg, wParam, lParam);
        #endif

        WmParamHandlerTable_CallHandlersOfId(currentWindowState.handlerTable, hwnd, msg, wParam, lParam);

        // Blit to the screen
        BitBlt(currentWindowState.hdc, 0, 0, currentWindowState.width, currentWindowState.height, currentWindowState.memHDC, 0, 0, SRCCOPY);

        // Clean up memory DC and bitmap
        SelectObject(currentWindowState.memHDC, hbmOld);
        DeleteObject(currentWindowState.memBitmap);
        DeleteDC(currentWindowState.memHDC);

        // End the paint process
        EndPaint(hwnd, &currentWindowState.ps);

        break;
    default:
        
        #ifndef DISABLE_MENU
        MenuUi_SubmenuCommandHandler(hwnd, msg, wParam, lParam);
        #endif

        if (WmParamHandlerTable_IdHasHandler(currentWindowState.handlerTable, msg)){
            
            WmParamHandlerTable_CallHandlersOfId(currentWindowState.handlerTable, hwnd, msg, wParam, lParam);
            
        }
        else {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        
    }
    return 0;
}