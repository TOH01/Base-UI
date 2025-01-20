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
} WindowState_t;

#endif