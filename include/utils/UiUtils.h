#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "common.h"
#include <windows.h>

#define UI_UTILS_CALCULATE_PERCENTAGE(value, percentage) ((value) * (percentage))
#define UI_UTILS_PERCENT(value) ((float)(value) / (100.0f))

#define buttonPos_t CommonPos_t

typedef enum {
	BORDER_NONE = 0,
	TOP = 1,
	RIGHT = 2,
	BOTTOM = 3,
	LEFT = 4,
} UiUtils_BorderEnum;

void UiUtils_DrawRectangleRelative(AbsolutePos_t pos);
bool UiUtils_CoordinateIsInsideOf(int x, int y, AbsolutePos_t pos);
UiUtils_BorderEnum UiUtils_CoordinateIsOnBorderOf(int x, int y, int borderWidth, AbsolutePos_t pos);
void UiUtils_FillRectangleRelative(AbsolutePos_t);
void UiUtils_DrawText(AbsolutePos_t pos, const char *name, UINT format);
HFONT UiUtils_CreateFont(void);
void UiUtils_DrawTextTheme(AbsolutePos_t pos, const char *name, UINT format, HFONT font, COLORREF color);
bool UiUtils_TextFitsBoxTheme(const char text[], AbsolutePos_t pos, HFONT font);
void UiUtils_DrawColoredRectangle(AbsolutePos_t pos, COLORREF colorBG, COLORREF colorBorder, int BorderSize);
int UiUtils_getLineHeight(HFONT font);
AbsolutePos_t getPosToContainer(const AbsolutePos_t *parentAbsPos, CommonPos_t widgetRelPos);
void UiUtils_DrawLineRelativeTheme(AbsolutePos_t pos, COLORREF color, int width);
RECT UiUtils_absolutePosToRect(AbsolutePos_t pos);
bool UiUtils_WidgetFitsInContainer(AbsolutePos_t widget, AbsolutePos_t container);
HFONT getFontForRect(HDC hdc, char text[], RECT box);
void UiUtils_DrawLineRelative(AbsolutePos_t pos);
AbsolutePos_t getPosToContainerAbsolute(const AbsolutePos_t *parentAbsPos, AbsolutePos_t widgetAbsPos);
void draw3SliceHelper(AbsolutePos_t pos, HICON left, HICON center, HICON right);
void draw9SliceHelper(AbsolutePos_t pos, HICON topLeft, HICON topCenter, HICON topRight, HICON midLeft, HICON center, HICON midRight, HICON bottomLeft, HICON bottomCenter, HICON bottomRight, int sliceWidth, int sliceHeight);

#endif