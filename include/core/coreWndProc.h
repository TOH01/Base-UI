#ifndef CORE_WND_PROC_H
#define CORE_WND_PROC_H

#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT NopHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT DefaultHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif