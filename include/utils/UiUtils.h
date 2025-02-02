#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <windows.h>
#include "common.h"

#define UI_UTILS_CALCULATE_PERCENTAGE(value, percentage) ((value) * (percentage))
#define UI_UTILS_PERCENT(value) ((float)(value) / (100.0f))

#define buttonPos_t CommonPos_t

typedef enum{
    TOP = 1,
    RIGHT = 2,
    BOTTOM = 3,
    LEFT = 4,
} Borders;


void UiUtils_CreatePens(void);
void UiUitls_DrawRectangleRelative(CommonPos_t pos);
bool UiUtils_CoordinateIsInsideOf(int x, int y, CommonPos_t pos);
int UiUtils_CoordinateIsOnBorderOf(int x, int y, int borderWidth, CommonPos_t pos);
void UiUtils_DrawRoundRectangleRelative(CommonPos_t pos);
void UiUtils_FillRectangleRelative(CommonPos_t pos);
void UiUtils_DrawEllipseRelative(CommonPos_t pos);
void UiUitls_DrawText(CommonPos_t pos, char name[]);

#endif