#ifndef TITLEBAR_H
#define TITLEBAR_H

#define _WIN32_WINNT 0x0A00

#include "common.h"
#include <uxtheme.h>
#include <vssym32.h>

#ifndef GET_X_PARAM
#define GET_X_PARAM(lp) ((int)(short)LOWORD(lp))
#endif

#ifndef GET_Y_PARAM
#define GET_Y_PARAM(lp) ((int)(short)HIWORD(lp))
#endif

int win32_dpi_scale(int value, UINT dpi);
RECT win32_titlebar_rect(HWND handle);
bool win32_window_is_maximized(HWND handle);
void drawTitlebar(HDC hdc, PAINTSTRUCT ps);

#endif