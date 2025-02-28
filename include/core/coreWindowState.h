#ifndef CORE_WINDOW_STATE_H
#define CORE_WINDOW_STATE_H

#include "WmParamHashTable.h"

typedef struct WindowState{
    int width;
    int height;
    WmParamHandlerTable_t * handlerTable;
    HDC hdc;
    HPEN hPen;
    PAINTSTRUCT ps;
    HDC memHDC;
    HBITMAP memBitmap;
    HWND hwnd;
} WindowState_t;

LRESULT CoreWindowState_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif