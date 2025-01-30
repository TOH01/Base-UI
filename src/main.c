#include <windows.h>
#include <stdio.h>
#include "main.h"
#include "WmParamHashTable.h"
#include "UiUtils.h"
#include "common.h"
#include "coreWndProc.h"

#ifndef DISABLE_MENU
#include "menuWindow.h"
#elif defined(DISABLE_MENU)
#include "singleWindow.h"
#endif

const char g_szClassName[] = "MyWindowClass";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    currentWindowState.wmParamHashTable = WmParamHandlerTable_Init();

    WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_CREATE, &CoreWindowState_WmCreateHook); // init handle in core window state

    InitialzeHandlers();

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
                          CW_USEDEFAULT, CW_USEDEFAULT, CONFIG_INIT_WINDOW_WIDTH, CONFIG_INIT_WINDOW_HEIGTH, NULL, NULL, hInstance, NULL);

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
