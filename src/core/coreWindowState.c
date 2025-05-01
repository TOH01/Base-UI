#include "coreWindowState.h"
#include "config.h"

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
}