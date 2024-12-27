#include "Common.h"
#include "coreWndProc.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"

LRESULT DefaultHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        WmParamHashTable_Destroy(currentWindowState.wmParamHashTable);
        PostQuitMessage(0);
        break;
    default:
        
        #ifndef DISABLE_MENU
        MenuUi_SubmenuCommandHandler(hwnd, msg, wParam, lParam);
        #endif

        MessageHandler_t handler = WmParamHashTable_Get(currentWindowState.wmParamHashTable, msg);

        if (handler){
            
            // if the handler isnt registered, return default handler
            
            if (handler == &DefaultHandler){
                return DefaultHandler(hwnd, msg, wParam, lParam);
            }
            
            // if the handler is registered, call it directly
            
            else {
                handler(hwnd, msg, wParam, lParam);
            }
             
        }
        else {
            
            // if a msg doesnt have a handler registered, point to a default handler so can prevent looping trough whole hash table each time

            WmParamHashTable_Insert(currentWindowState.wmParamHashTable, msg, &DefaultHandler);

            return DefaultHandler(hwnd, msg, wParam, lParam);
        }
        
    }
    return 0;
}