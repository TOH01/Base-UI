#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "common.h"
#include <windows.h>

#define UI_UTILS_CALCULATE_PERCENTAGE(value, percentage) ((value) * (percentage))
#define UI_UTILS_PERCENT(value) ((float)(value) / (100.0f))

#define buttonPos_t CommonPos_t

typedef enum {
	TOP = 1,
	RIGHT = 2,
	BOTTOM = 3,
	LEFT = 4,
} UiUtils_BorderEnum;

void UiUtils_CreatePens(void);
void UiUtils_DrawRectangleRelative(CommonPos_t pos);
bool UiUtils_CoordinateIsInsideOf(int x, int y, CommonPos_t pos);
UiUtils_BorderEnum UiUtils_CoordinateIsOnBorderOf(int x, int y, int borderWidth, CommonPos_t pos);
void UiUtils_DrawRoundRectangleRelative(CommonPos_t pos);
void UiUtils_FillRectangleRelative(CommonPos_t pos);
void UiUtils_DrawEllipseRelative(CommonPos_t pos);
void UiUtils_DrawText(CommonPos_t pos, char *name, UINT format);
bool UiUtils_TextFitsBox(char text[], CommonPos_t pos);
RECT UiUtils_CommonPosToRect(CommonPos_t pos);
CommonPos_t UiUtils_RectToCommonsPos(RECT rect);
HFONT UiUtils_CreateFont();
void UiUtils_DrawTextTheme(CommonPos_t pos, char *name, UINT format, HFONT font, COLORREF color);
bool UiUtils_TextFitsBoxTheme(char text[], CommonPos_t pos, HFONT font);
void UiUtils_DrawFilledRectangle(CommonPos_t pos, COLORREF color);
void UiUtils_DrawColoredRectangle(CommonPos_t pos, COLORREF colorBG, COLORREF colorBorder, int BorderSize);
int UiUtils_getLineHeight(HFONT font);
CommonPos_t getPosToContainer(CommonPos_t containerPos, CommonPos_t widgetPos);

#endif