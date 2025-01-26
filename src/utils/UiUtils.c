#include <windows.h>
#include "UiUtils.h"
#include "common.h"


void UiUtils_CreatePens(void){
    currentWindowState.hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
}

HWND UiUtils_initButton(HWND hwnd, button_t button){
    return CreateWindow(
                "BUTTON", 
                button.name, 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                UI_UTILS_CALCULATE_PERCENTAGE(CONFIG_INIT_WINDOW_WIDTH, button.pos.spacingLeft),
                UI_UTILS_CALCULATE_PERCENTAGE(CONFIG_INIT_WINDOW_HEIGTH, button.pos.spacingTop), 
                UI_UTILS_CALCULATE_PERCENTAGE(CONFIG_INIT_WINDOW_WIDTH, button.pos.width), 
                UI_UTILS_CALCULATE_PERCENTAGE(CONFIG_INIT_WINDOW_HEIGTH, button.pos.height), 
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

void UiUitls_DrawRectangleRelative(CommonPos_t pos){
    Rectangle(currentWindowState.memHDC, UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentWidth, pos.spacingLeft), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentHeight, pos.spacingTop), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentWidth, pos.width), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentHeight, pos.height));
}

void UiUtils_DrawRoundRectangleRelative(CommonPos_t pos){
    RoundRect(currentWindowState.memHDC, UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentWidth, pos.spacingLeft), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentHeight, pos.spacingTop), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentWidth, pos.width), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentHeight, pos.height), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentHeight, 0.2 * ((pos.height - pos.spacingTop) + (pos.width - pos.spacingLeft) / 2)), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentHeight, 0.2 * ((pos.height - pos.spacingTop) + (pos.width - pos.spacingLeft) / 2)));
}

void UiUtils_FillRectangleRelative(CommonPos_t pos){
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
    
    RECT rect = {UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentWidth, pos.spacingLeft), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentHeight, pos.spacingTop), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentWidth, pos.width), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.currentHeight, pos.height)};

    FillRect(currentWindowState.memHDC, &rect, hBrush);
}

bool UiUtils_CoordinateIsInsideOf(int x, int y, CommonPos_t pos){
    if (x > pos.spacingLeft * currentWindowState.currentWidth && x < pos.width * currentWindowState.currentWidth){
        if (y > pos.spacingTop * currentWindowState.currentHeight && y < pos.height * currentWindowState.currentHeight){
            return true;
        }
    }
    return false;
}

int UiUtils_CoordinateIsOnBorderOf(int x, int y, int borderWidth, CommonPos_t pos){
    // check if on left border
    if (x >= (pos.spacingLeft - 0.005f) * currentWindowState.currentWidth && (x + borderWidth) <= (pos.spacingLeft + 0.005f) * currentWindowState.currentWidth){
        if (y >= pos.spacingTop * currentWindowState.currentHeight && y <= pos.height * currentWindowState.currentHeight){
            return LEFT;
        }
    }

    //check if on right border
    if (x >= (pos.width - 0.005f) * currentWindowState.currentWidth && (x + borderWidth) <= (pos.width + 0.005f) * currentWindowState.currentWidth){
        if (y >= pos.spacingTop * currentWindowState.currentHeight && y <= pos.height * currentWindowState.currentHeight){
            return RIGHT;
        }
    }

    //check if on bottom border
    if (y >= (pos.height - 0.005f) * currentWindowState.currentHeight && (y - borderWidth) <= (pos.height + 0.005f) * currentWindowState.currentHeight){
        if(x >= pos.spacingLeft * currentWindowState.currentWidth && x <= pos.width * currentWindowState.currentWidth) {
            return BOTTOM;
        }
    }

    //check if on top border
    if (y >= (pos.spacingTop - 0.005f) * currentWindowState.currentHeight && (y - borderWidth) <= (pos.spacingTop + 0.005f) * currentWindowState.currentHeight){
        if(x >= pos.spacingLeft * currentWindowState.currentWidth && x <= pos.width * currentWindowState.currentWidth) {
            return TOP;
        }
    }

    return false;
}
