#ifndef CORE_WND_PROC_H
#define CORE_WND_PROC_H

#include <windows.h>

typedef LRESULT (*MessageHook)(HWND, UINT, WPARAM, LPARAM);

void SetWmCreateHook(MessageHook userHook);
void SetWmSizeHook(MessageHook userHook);
void SetWmPaintHook(MessageHook userHook);
void SetWmCommandHook(MessageHook userHook);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif