#include "Common.h"
#include "coreWndProc.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"

LRESULT DefaultHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT NopHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    //do nothing, used to improve performance
}

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
    default:
        
        #ifndef DISABLE_MENU
        MenuUi_SubmenuCommandHandler(hwnd, msg, wParam, lParam);
        #endif

        if (WmParamHandlerTable_IdHasHandler(currentWindowState.wmParamHashTable, msg)){
            
            // if the handler isnt registered, return default handler
            
            if (!WmParamHandlerTable_IdHasNonDefaultHandler(currentWindowState.wmParamHashTable, msg)){
                return DefaultHandler(hwnd, msg, wParam, lParam);
            }
            
            // if the handler is registered, call it directly
            
            else {
                WmParamHandlerTable_CallHandlersOfId(currentWindowState.wmParamHashTable, hwnd, msg, wParam, lParam);
            }
             
        }
        else {
            
            // if a msg doesnt have a handler registered, point to a default handler so can prevent looping trough whole hash table each time

            WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, msg, &DefaultHandler);

            return DefaultHandler(hwnd, msg, wParam, lParam);
        }
        
    }
    return 0;
}