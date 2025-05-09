#include "coreWindowState.h"
#include "config.h"
#include "titlbar.h"

WindowState_t currentWindowState = {
    .width = CONFIG_INIT_WINDOW_WIDTH,
    .height = CONFIG_INIT_WINDOW_HEIGTH,
    .handlerTable = NULL,
    .hPen = NULL,
    .ps = NULL,
    .hdc = NULL,
    .activeTheme = DARKMODE_THEME(),
};

LRESULT CoreWindowState_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	currentWindowState.hwnd = hwnd;
#ifdef CUSTOM_TITLE_BAR
	SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
#endif
	InvalidateRect(hwnd, NULL, FALSE);
}

LRESULT CoreWindowState_WmSizeHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	InvalidateRect(hwnd, NULL, FALSE);

	if (LOWORD(lParam) > CONFIG_MIN_WINDOW_WIDTH) {
		currentWindowState.width = LOWORD(lParam);
	}
	if (HIWORD(lParam) > CONFIG_MIN_WINDOW_Height) {
		currentWindowState.height = HIWORD(lParam);

#ifdef CUSTOM_TITLE_BAR

		currentWindowState.titlbarHeight = getTitleBarHeight(hwnd);

#endif
	}
}