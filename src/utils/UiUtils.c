#include "UiUtils.h"
#include "common.h"
#include "string.h"
#include <windows.h>

RECT UiUtils_CommonPosToRect(CommonPos_t pos) {

#ifndef CUSTOM_TITLE_BAR
	RECT rect = {UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.left), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.top), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.right), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height, pos.bottom)};
#elif defined(CUSTOM_TITLE_BAR)
	RECT rect = {UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.left), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height - currentWindowState.titlbarHeight, pos.top) + currentWindowState.titlbarHeight, UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.width, pos.right), UI_UTILS_CALCULATE_PERCENTAGE(currentWindowState.height - currentWindowState.titlbarHeight , pos.bottom) + currentWindowState.titlbarHeight};
#endif

	return rect;
}

CommonPos_t UiUtils_RectToCommonsPos(RECT rect) {
	CommonPos_t pos = {(rect.top / (float)currentWindowState.height), (rect.left / (float)currentWindowState.width), (rect.right / (float)currentWindowState.width), (rect.bottom / (float)currentWindowState.height)};

	return pos;
}

void UiUtils_CreatePens(void) { currentWindowState.hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0)); }

void UiUtils_DrawRectangleRelative(CommonPos_t pos) {

	RECT rect = UiUtils_CommonPosToRect(pos);

	Rectangle(currentWindowState.memHDC, rect.left, rect.top, rect.right, rect.bottom);
}

void UiUtils_DrawFilledRectangle(CommonPos_t pos, COLORREF color) {
	HBRUSH fillBrush = CreateSolidBrush(color);

	RECT fillRect = UiUtils_CommonPosToRect(pos);

	FillRect(currentWindowState.memHDC, &fillRect, fillBrush);
	DeleteObject(fillBrush);
}

void UiUtils_DrawColoredRectangle(CommonPos_t pos, COLORREF colorBG, COLORREF colorBorder, int BorderSize) {

	HPEN pen = CreatePen(PS_SOLID, BorderSize, colorBorder);
	HPEN oldPen = SelectObject(currentWindowState.memHDC, pen);

	HBRUSH fillBrush = CreateSolidBrush(colorBG);
	HBRUSH oldBrush = SelectObject(currentWindowState.memHDC, fillBrush);

	UiUtils_DrawRectangleRelative(pos);

	SelectObject(currentWindowState.memHDC, oldBrush);
	SelectObject(currentWindowState.memHDC, oldPen);
	DeleteObject(pen);
	DeleteObject(fillBrush);
}

void UiUtils_DrawText(CommonPos_t pos, char *name, UINT format) {

	RECT textRect = UiUtils_CommonPosToRect(pos);

	DrawText(currentWindowState.memHDC, name, -1, &textRect, format);
}

void UiUtils_DrawLineRelative(CommonPos_t pos, COLORREF color, int width) {
	HPEN pen = CreatePen(PS_SOLID, width, color);
	HPEN oldPen = SelectObject(currentWindowState.memHDC, pen);

	RECT r = UiUtils_CommonPosToRect(pos);

	MoveToEx(currentWindowState.memHDC, r.left, r.top, NULL);
	LineTo(currentWindowState.memHDC, r.right, r.bottom);

	SelectObject(currentWindowState.memHDC, oldPen);
	DeleteObject(pen);
}

void UiUtils_DrawTextTheme(CommonPos_t pos, char *name, UINT format, HFONT font, COLORREF color) {

	SetTextColor(currentWindowState.memHDC, color);
	SetBkMode(currentWindowState.memHDC, TRANSPARENT);

	HFONT oldFont = SelectObject(currentWindowState.memHDC, font);

	SIZE textSize;
	GetTextExtentPoint32A(currentWindowState.memHDC, name, strlen(name), &textSize);

	float textHeightRel = (float)textSize.cy / currentWindowState.height;

	float boxHeightRel = pos.bottom - pos.top;
	float offsetRel = (boxHeightRel - textHeightRel) / 2.0f;

	CommonPos_t adjustedPos = pos;
	adjustedPos.top = pos.top + offsetRel;
	adjustedPos.bottom = adjustedPos.top + textHeightRel;

	UiUtils_DrawText(adjustedPos, name, format);

	SelectObject(currentWindowState.memHDC, oldFont);
}

void UiUtils_FillRectangleRelative(CommonPos_t pos) {
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));

	RECT rect = UiUtils_CommonPosToRect(pos);

	FillRect(currentWindowState.memHDC, &rect, hBrush);
}

bool UiUtils_CoordinateIsInsideOf(int x, int y, CommonPos_t pos) {

	RECT rect = UiUtils_CommonPosToRect(pos);

	if (x > rect.left && x < rect.right) {
		if (y > rect.top && y < rect.bottom) {
			return true;
		}
	}
	return false;
}

bool UiUtils_TextFitsBox(char text[], CommonPos_t pos) {

	SIZE textSize;
	RECT rect = UiUtils_CommonPosToRect(pos);

	GetTextExtentPoint32(currentWindowState.memHDC, text, strlen(text), &textSize);

	if (textSize.cx > ((rect.right) - (rect.left))) {
		return false;
	}

	return true;
}

int UiUtils_getLineHeight(HFONT font) {

	TEXTMETRIC tm;
	HFONT oldFont = SelectObject(currentWindowState.hdc, font);

	GetTextMetrics(currentWindowState.hdc, &tm);

	// Restore the old font
	SelectObject(currentWindowState.hdc, oldFont);

	return tm.tmHeight;
}

bool UiUtils_TextFitsBoxTheme(char text[], CommonPos_t pos, HFONT font) {
	SIZE textSize;

	HDC hdc = currentWindowState.memHDC;

	// Temporarily select the font into the DC
	HFONT oldFont = SelectObject(hdc, font);

	// Measure the text
	GetTextExtentPoint32A(hdc, text, strlen(text), &textSize);

	// Restore the old font
	SelectObject(hdc, oldFont);

	RECT rect = UiUtils_CommonPosToRect(pos);

	int boxWidth = rect.right - rect.left;
	int boxHeight = rect.bottom - rect.top;

	bool fitsWidth = textSize.cx <= boxWidth;
	bool fitsHeight = textSize.cy <= boxHeight;

	return fitsWidth && fitsHeight;
}

HFONT UiUtils_CreateFont() { return CreateFont(-MulDiv(10, GetDeviceCaps(currentWindowState.memHDC, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Segoe UI")); }

UiUtils_BorderEnum UiUtils_CoordinateIsOnBorderOf(int x, int y, int borderWidth, CommonPos_t pos) {
	// check if on left border
	
	RECT rect = UiUtils_CommonPosToRect(pos);
	
	if (x >= rect.left && (x + borderWidth) <= rect.left) {
		if (y >= rect.top && y <= rect.bottom) {
			return LEFT;
		}
	}

	// check if on right border
	if (x >= rect.right && (x + borderWidth) <= rect.right) {
		if (y >= rect.top && y <= rect.bottom) {
			return RIGHT;
		}
	}

	// check if on bottom border
	if (y >= rect.bottom && (y - borderWidth) <= rect.bottom) {
		if (x >= rect.left && x <= rect.right) {
			return BOTTOM;
		}
	}

	// check if on top border
	if (y >= rect.top && (y - borderWidth) <= rect.top) {
		if (x >= rect.left && x <= rect.right) {
			return TOP;
		}
	}

	return false;
}

CommonPos_t getPosToContainer(CommonPos_t containerPos, CommonPos_t widgetPos) {
	CommonPos_t pos;

	pos.left = containerPos.left + ((containerPos.right - containerPos.left) * widgetPos.left);
	pos.right = containerPos.left + ((containerPos.right - containerPos.left) * widgetPos.right);

	pos.top = containerPos.top + ((containerPos.bottom - containerPos.top) * widgetPos.top);
	pos.bottom = containerPos.top + ((containerPos.bottom - containerPos.top) * widgetPos.bottom);

	return pos;
}