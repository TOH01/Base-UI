#include "main.h"
#include "UiUtils.h"
#include "WmParamHashTable.h"
#include "common.h"
#include "container.h"
#include "coreWndProc.h"
#include <stdio.h>
#include <windows.h>

#include "demo.h"

#ifdef CUSTOM_TITLE_BAR
#include "titlbar.h"
#endif

const char g_szClassName[] = "MyWindowClass";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	(void)hPrevInstance;
	(void)lpCmdLine;

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	currentWindowState.handlerTable = WmParamHandlerTable_Init();
	currentWindowState.hInstance = hInstance;

	WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_CREATE, &CoreWindowState_WmCreateHook); // init handler in core window state
	WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_SIZE, &CoreWindowState_WmSizeHook);     // resize handler in core window state

	UiTheme_initFonts();

	initRootContainer();

	Demo_InitAll();

#ifdef CUSTOM_TITLE_BAR
	initTitlebar();
#endif

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

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	int window_style = WS_THICKFRAME    // required for a standard resizeable window
	                   | WS_SYSMENU     // Explicitly ask for the titlebar to support snapping via Win + ← / Win + →
	                   | WS_MAXIMIZEBOX // Add maximize button to support maximizing via mouse dragging
	                                    // to the top of the screen
	                   | WS_MINIMIZEBOX // Add minimize button to support minimizing by clicking on the taskbar icon
	                   | WS_VISIBLE;    // Make window visible after it is created (not important)

	hwnd = CreateWindowEx(0, g_szClassName, NULL, window_style, CW_USEDEFAULT, CW_USEDEFAULT, CONFIG_INIT_WINDOW_WIDTH, CONFIG_INIT_WINDOW_HEIGTH, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
