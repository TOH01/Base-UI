#include "Common.h"
#include "coreWndProc.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        WmParamHandlerTable_Destroy(currentWindowState.wmParamHashTable);
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        
        currentWindowState.hdc = BeginPaint(hwnd, &currentWindowState.ps);

        #ifndef DISABLE_MENU
        WmParamHandlerTable_CallHandlersOfId(getGurrentSubmenu()->WmParamHashTable, hwnd, msg, wParam, lParam);
        #endif

        WmParamHandlerTable_CallHandlersOfId(currentWindowState.wmParamHashTable, hwnd, msg, wParam, lParam);

        EndPaint(hwnd, &currentWindowState.ps);

        break;
    default:
        
        #ifndef DISABLE_MENU
        MenuUi_SubmenuCommandHandler(hwnd, msg, wParam, lParam);
        #endif

        if (msg == WM_SIZE){
            InvalidateRect(hwnd, NULL, TRUE); // when windows is resized, send message for WM_PAINT handler to redraw
        }

        if (WmParamHandlerTable_IdHasHandler(currentWindowState.wmParamHashTable, msg)){
            
            WmParamHandlerTable_CallHandlersOfId(currentWindowState.wmParamHashTable, hwnd, msg, wParam, lParam);
            
        }
        else {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        
    }
    return 0;
}