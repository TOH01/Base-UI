#ifndef CORE_WINDOW_STATE_H
#define CORE_WINDOW_STATE_H

#include "WmParamHashTable.h"

typedef struct WindowState{
    int currentWidth;
    int currentHeight;
    WmParamHandlerTable_t * wmParamHashTable;
    HDC hdc;
    HPEN hPen;
    PAINTSTRUCT ps;
    HDC memHDC;
    HBITMAP memBitmap;
    HWND hwnd;
} WindowState_t;

LRESULT CoreWindowState_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif