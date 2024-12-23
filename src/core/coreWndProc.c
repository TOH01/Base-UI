#include "Common.h"
#include "coreWndProc.h"
#include "WmParamHashTable.h"

/*hooks for commonly used messages*/

static MessageHook WmCreateHook = NULL;
static MessageHook WmSizeHook = NULL;
static MessageHook WmPaintHook = NULL;
static MessageHook WmCommandHook = NULL;

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
        
        MessageHandler_t handler = WmParamHashTable_Get(currentWindowState.wmParamHashTable, msg);

        if (handler){
            
            if (handler == &DefaultHandler){
                return handler(hwnd, msg, wParam, lParam);
            }
            else {
                handler(hwnd, msg, wParam, lParam);
            }
             
        }
        else {
            
            // if a msg doesnt have a handler registered, point to a default handler so can prevent looping trough whole hash table each time

            WmParamHashTable_Insert(currentWindowState.wmParamHashTable, msg, &DefaultHandler);

            return WmParamHashTable_Get(currentWindowState.wmParamHashTable, msg)(hwnd, msg, wParam, lParam);
        }
        
    }
    return 0;
}