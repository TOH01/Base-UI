#include "coreWindowState.h"
#include "config.h"

WindowState_t currentWindowState = {
    .currentWidth = CONFIG_INIT_WINDOW_WIDTH,
    .currentHeight = CONFIG_INIT_WINDOW_HEIGTH,
    .wmParamHashTable = NULL,
    .hPen = NULL,
    .ps = NULL,
    .hdc = NULL,
};

LRESULT CoreWindowState_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    currentWindowState.hwnd = hwnd;
}