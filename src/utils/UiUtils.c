#include <windows.h>
#include "UiUtils.h"

PAINTSTRUCT ps;
HDC hdc;
HPEN hPen;

void UiUtils_CreatePens(){
    hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
}

HWND UiUtils_initButton(HWND hwnd, button_t button){
    return CreateWindow(
                "BUTTON", 
                button.name, 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                UI_UTILS_CALCULATE_PERCENTAGE(UI_UTILS_INIT_WINDOW_WIDTH, button.pos.spacingLeft),
                UI_UTILS_CALCULATE_PERCENTAGE(UI_UTILS_INIT_WINDOW_HEIGTH, button.pos.spacingTop), 
                UI_UTILS_CALCULATE_PERCENTAGE(UI_UTILS_INIT_WINDOW_WIDTH, button.pos.width), 
                UI_UTILS_CALCULATE_PERCENTAGE(UI_UTILS_INIT_WINDOW_HEIGTH, button.pos.height), 
                hwnd, 
                (HMENU)button.wParam,
                NULL, NULL
            );
}

void UiUtils_moveButton(HWND hwnd, buttonPos_t pos, int width, int height){
    MoveWindow(
        hwnd,
        UI_UTILS_CALCULATE_PERCENTAGE(width, pos.spacingLeft),
        UI_UTILS_CALCULATE_PERCENTAGE(height, pos.spacingTop),
        UI_UTILS_CALCULATE_PERCENTAGE(width, pos.width),
        UI_UTILS_CALCULATE_PERCENTAGE(height, pos.height),
        TRUE
    );
}

#include "main.h"
#include <stdio.h>

