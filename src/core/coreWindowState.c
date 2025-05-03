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
    .activeTheme = OCEAN_BREEZE_THEME(),
};

LRESULT CoreWindowState_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	currentWindowState.hwnd = hwnd;
#ifdef CUSTOM_TITLE_BAR
	SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
#endif
}