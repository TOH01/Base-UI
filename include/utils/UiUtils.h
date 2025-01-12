#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <windows.h>
#include "common.h"

#define UI_UTILS_CALCULATE_PERCENTAGE(value, percentage) ((value) * (percentage) / 100)


#define buttonPos_t CommonPos_t

typedef struct button{
    buttonPos_t pos;
    char name[30];
    int wParam;
} button_t;

HWND UiUtils_initButton(HWND hwnd, button_t button);
void UiUtils_moveButton(HWND hwnd, buttonPos_t pos, int width, int height);
void UiUtils_CreatePens(void);
void UiUitls_DrawRectangleRelative(int x, int y, int width, int height);

#endif