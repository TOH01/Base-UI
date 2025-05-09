#ifndef CORE_WINDOW_STATE_H
#define CORE_WINDOW_STATE_H

#include "UiTheme.h"
#include "WmParamHashTable.h"
#include "DynamicArray.h"
#include "config.h"

typedef struct WindowState {
	int width;
	int height;
	WmParamHandlerTable_t *handlerTable;
	HDC hdc;
	HPEN hPen;
	PAINTSTRUCT ps;
	HDC memHDC;
	HBITMAP memBitmap;
	HWND hwnd;
	UiTheme_t activeTheme;
	DynamicArray_t * containers;
	#ifdef CUSTOM_TITLE_BAR
	int titlbarHeight;
	#endif
} WindowState_t;

LRESULT CoreWindowState_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CoreWindowState_WmSizeHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif