#include <windows.h>
#include <stdio.h>
#include "main.h"
#include "WmParamHashTable.h"

const char g_szClassName[] = "MyWindowClass";

int currentHeight = UI_UTILS_INIT_WINDOW_HEIGTH;
int currentWidth = UI_UTILS_INIT_WINDOW_WIDTH;

HashTable_t *MessageHashTable;

typedef LRESULT (*MessageHook)(HWND, UINT, WPARAM, LPARAM);

static MessageHook WmCreateHook = NULL;
static MessageHook WmSizeHook = NULL;
static MessageHook WmPaintHook = NULL;
static MessageHook WmCommandHook = NULL;

void SetWmCreateHook(MessageHook userHook)
{
    WmCreateHook = userHook;
}

void SetWmSizeHook(MessageHook userHook)
{
    WmSizeHook = userHook;
}

void SetWmPaintHook(MessageHook userHook)
{
    WmPaintHook = userHook;
}

void SetWmCommandHook(MessageHook userHook)
{
   WmCommandHook = userHook;
}

LRESULT DefaultHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#ifdef DISABLE_MENU

void InitializeHooks(){
    SetWmCreateHook(NULL);
    SetWmPaintHook(NULL);
    SetWmSizeHook(NULL);
    SetWmCommandHook(NULL);
}

#endif

#ifndef DISABLE_MENU

#include "MenuUi.h"

LRESULT MenuUi_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    
    MenuUi_RenderMenuButtons(hwnd);

    MenuUi_SubmenuSwap(hwnd, 0);

    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT MenuUi_WmSizeHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (currentWidth < UI_UTILS_MIN_WINDOW_WIDTH)
    {
        currentWidth = UI_UTILS_MIN_WINDOW_WIDTH;
    }
    if (currentHeight < UI_UTILS_MIN_WINDOW_Height)
    {
        currentHeight = UI_UTILS_MIN_WINDOW_Height;
    }

    MenuUi_ResizeMenuUi(currentWidth, currentHeight);

    MenuUi_ResizeMenuContent(currentWidth, currentHeight);

    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT MenuUi_WmPaintHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MenuUi_DrawMenuUi(hwnd, currentWidth, currentHeight);

    MenuUi_DrawMenuContent(hwnd, currentWidth, currentHeight);
}

LRESULT MenuUi_WmCommandHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (MenuUi_IsSidebarCommand(LOWORD(wParam)))
    {
        MenuUi_SubmenuSwap(hwnd, LOWORD(wParam));
        SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(currentWidth, currentHeight));
    }

    else
    {
        MenuUi_SubmenuWmParamCallback(hwnd, wParam);
    }
}

void InitializeHooks(){
    
    SetWmCreateHook(&MenuUi_WmCreateHook);
    SetWmPaintHook(&MenuUi_WmPaintHook);
    SetWmSizeHook(&MenuUi_WmSizeHook);
    SetWmCommandHook(&MenuUi_WmCommandHook);
}

#endif

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        WmParamHashTable_Destroy(MessageHashTable);
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        
        if (WmCreateHook){
            WmCreateHook(hwnd, msg, wParam, lParam);
        }

        break;
    case WM_SIZE:
        currentWidth = LOWORD(lParam);
        currentHeight = HIWORD(lParam);

        if (WmSizeHook){
            WmSizeHook(hwnd, msg, wParam, lParam);
        }

        break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        if (WmPaintHook){
            WmPaintHook(hwnd, msg, wParam, lParam);
        }

        EndPaint(hwnd, &ps);
        break;
    case WM_COMMAND:

        if (WmCommandHook){
            WmCommandHook(hwnd, msg, wParam, lParam);
        }
        break;
    default:
        
        MessageHandler_t handler = WmParamHashTable_Get(MessageHashTable, msg);

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
            // TODO: too many unique msg id might fill up the hashtable and cause buffer overlow!
            WmParamHashTable_Insert(MessageHashTable, msg, &DefaultHandler);

            return WmParamHashTable_Get(MessageHashTable, msg)(hwnd, msg, wParam, lParam);
        }
        
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    InitializeHooks();

    MessageHashTable = WmParamHashTable_Init();

    #ifndef DISABLE_MENU

    MenuUi_SubmenuInitAll();

    #endif

    UiUtils_CreatePens();

    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(0, g_szClassName, "State Machine GUI", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, UI_UTILS_INIT_WINDOW_WIDTH, UI_UTILS_INIT_WINDOW_HEIGTH, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
