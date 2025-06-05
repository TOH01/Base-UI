#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "common.h"
#include <uxtheme.h>
#include <vssym32.h>

#ifdef CUSTOM_TITLE_BAR

typedef enum{
    NONE,
    CLOSE,
    RESTORE,
    MINIMIZE,
} activeHover;

int win32_dpi_scale(int value, UINT dpi);
RECT win32_titlebar_rect(HWND handle);
bool win32_window_is_maximized(HWND handle);
void drawTitlebar(HDC hdc);
int getTitleBarHeight(HWND handle);
LRESULT Titlebar_WmNCLButtonDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT MenuUi_WmNCLButtonUp(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT Titlebar_WmNCMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void initTitlebar(void);

#endif

#endif
