#include <windows.h>
#include "UiUtils.h"
#include "common.h"
#include "string.h"

RECT UiUtils_CommonPosToRect(CommonPos_t pos){
    
    RECT rect = {UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.left), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.top), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.right), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.bottom)};
    
    return rect;
}

CommonPos_t UiUtils_RectToCommonsPos(RECT rect){
    CommonPos_t pos = {(rect.top / (float)currentWindowState.height), (rect.left / (float)currentWindowState.width), (rect.right / (float)currentWindowState.width), (rect.bottom / (float)currentWindowState.height)};

    return pos;
}


void UiUtils_CreatePens(void){
    currentWindowState.hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
}

void UiUtils_DrawRectangleRelative(CommonPos_t pos){
    Rectangle(currentWindowState.memHDC, UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.left), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.top), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.right), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.bottom));
}

void UiUtils_DrawColoredRectangle(CommonPos_t pos, COLORREF color){
    
    HPEN pen = CreatePen(PS_SOLID, 2, color);
    HPEN oldPen = SelectObject(currentWindowState.memHDC, pen);

    UiUtils_DrawRectangleRelative(pos);

    SelectObject(currentWindowState.memHDC, oldPen);
    DeleteObject(pen);
}

void UiUtils_DrawRoundRectangleRelative(CommonPos_t pos){
    RoundRect(currentWindowState.memHDC, UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.left), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.top), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.right), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.bottom), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, 0.2 * ((pos.bottom - pos.top) + (pos.right - pos.left) / 2)), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, 0.2 * ((pos.bottom - pos.top) + (pos.right - pos.left) / 2)));
}

void UiUtils_DrawEllipseRelative(CommonPos_t pos){
    Ellipse(currentWindowState.memHDC, UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.left), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.top), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.right), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.bottom));
}

void UiUtils_DrawText(CommonPos_t pos, char * name, UINT format){
    
    // multipliers to temporarily adjust for border witdh
    RECT textRect = {
        UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.left) * 1.02, 
        UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.top) * 1.02, 
        UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.right) * 0.98, 
        UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.bottom) * 0.98,    
    };

    DrawText(currentWindowState.memHDC, name, -1, &textRect, format);  // Left align, wrap words at edges

}

void UiUtils_FillRectangleRelative(CommonPos_t pos){
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
    
    RECT rect = UiUtils_CommonPosToRect(pos);

    FillRect(currentWindowState.memHDC, &rect, hBrush);

}

bool UiUtils_CoordinateIsInsideOf(int x, int y, CommonPos_t pos){
    if (x > pos.left * currentWindowState.width && x < pos.right * currentWindowState.width){
        if (y > pos.top * currentWindowState.height && y < pos.bottom * currentWindowState.height){
            return true;
        }
    }
    return false;
}

bool UiUtils_TextFitsBox(char text[], CommonPos_t pos){
    
    SIZE textSize;
    GetTextExtentPoint32(currentWindowState.memHDC, text, strlen(text), &textSize);

    // multipliers to temporarily adjust for border witdh
    if (textSize.cx > ((pos.right * currentWindowState.width) * 0.98 - (pos.left * currentWindowState.width) * 1.02)) {
        return false;
    }

    return true;
}

UiUtils_BorderEnum UiUtils_CoordinateIsOnBorderOf(int x, int y, int borderWidth, CommonPos_t pos){
    // check if on left border
    if (x >= (pos.left - 0.005f) * currentWindowState.width && (x + borderWidth) <= (pos.left + 0.005f) * currentWindowState.width){
        if (y >= pos.top * currentWindowState.height && y <= pos.bottom * currentWindowState.height){
            return LEFT;
        }
    }

    //check if on right border
    if (x >= (pos.right - 0.005f) * currentWindowState.width && (x + borderWidth) <= (pos.right + 0.005f) * currentWindowState.width){
        if (y >= pos.top * currentWindowState.height && y <= pos.bottom * currentWindowState.height){
            return RIGHT;
        }
    }

    //check if on bottom border
    if (y >= (pos.bottom - 0.005f) * currentWindowState.height && (y - borderWidth) <= (pos.bottom + 0.005f) * currentWindowState.height){
        if(x >= pos.left * currentWindowState.width && x <= pos.right * currentWindowState.width) {
            return BOTTOM;
        }
    }

    //check if on top border
    if (y >= (pos.top - 0.005f) * currentWindowState.height && (y - borderWidth) <= (pos.top + 0.005f) * currentWindowState.height){
        if(x >= pos.left * currentWindowState.width && x <= pos.right * currentWindowState.width) {
            return TOP;
        }
    }

    return false;
}

