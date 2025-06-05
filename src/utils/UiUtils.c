#include "UiUtils.h"
#include "common.h"
#include "string.h"
#include <windows.h>

RECT UiUtils_absolutePosToRect(AbsolutePos_t pos) {
	RECT rect;
	rect.left = pos.left;
	rect.top = pos.top;
	rect.right = pos.right;
	rect.bottom = pos.bottom;
	return rect;
}

void UiUtils_DrawRectangleRelative(AbsolutePos_t pos) { Rectangle(currentWindowState.memHDC, pos.left, pos.top, pos.right, pos.bottom); }

void UiUtils_DrawColoredRectangle(AbsolutePos_t pos, COLORREF colorBG, COLORREF colorBorder, int BorderSize) {

	HPEN pen = CreatePen(PS_SOLID, BorderSize, colorBorder);

	if (BorderSize == 0) {
		DeleteObject(pen);
		pen = (HPEN)GetStockObject(NULL_PEN);
	}

	HPEN oldPen = SelectObject(currentWindowState.memHDC, pen);

	HBRUSH fillBrush = CreateSolidBrush(colorBG);
	HBRUSH oldBrush = SelectObject(currentWindowState.memHDC, fillBrush);

	UiUtils_DrawRectangleRelative(pos);

	SelectObject(currentWindowState.memHDC, oldBrush);
	SelectObject(currentWindowState.memHDC, oldPen);
	DeleteObject(pen);
	DeleteObject(fillBrush);
}

void UiUtils_DrawText(AbsolutePos_t pos, const char *name, UINT format) {

	RECT textRect = UiUtils_absolutePosToRect(pos);

	DrawText(currentWindowState.memHDC, name, -1, &textRect, format);
}

void UiUtils_DrawLineRelative(AbsolutePos_t pos) {
	RECT r = UiUtils_absolutePosToRect(pos);

	MoveToEx(currentWindowState.memHDC, r.left, r.top, NULL);
	LineTo(currentWindowState.memHDC, r.right, r.bottom);
}

void UiUtils_DrawLineRelativeTheme(AbsolutePos_t pos, COLORREF color, int width) {
	HPEN pen = CreatePen(PS_SOLID, width, color);
	HPEN oldPen = SelectObject(currentWindowState.memHDC, pen);

	UiUtils_DrawLineRelative(pos);

	SelectObject(currentWindowState.memHDC, oldPen);
	DeleteObject(pen);
}

void UiUtils_DrawTextTheme(AbsolutePos_t pos, const char *name, UINT format, HFONT font, COLORREF color) {

	SetTextColor(currentWindowState.memHDC, color);
	SetBkMode(currentWindowState.memHDC, TRANSPARENT);

	HFONT oldFont = SelectObject(currentWindowState.memHDC, font);

	SIZE textSize;
	GetTextExtentPoint32A(currentWindowState.memHDC, name, strlen(name), &textSize);

	int boxHeight = pos.bottom - pos.top;
	int offsetY = (boxHeight - textSize.cy) / 2;

	pos.top += offsetY;
	pos.bottom += pos.top + textSize.cy;

	UiUtils_DrawText(pos, name, format);

	SelectObject(currentWindowState.memHDC, oldFont);
}

bool UiUtils_CoordinateIsInsideOf(int x, int y, AbsolutePos_t pos) { return (x > pos.left && x < pos.right && y > pos.top && y < pos.bottom); }

int UiUtils_getLineHeight(HFONT font) {

	TEXTMETRIC tm;
	HFONT oldFont = SelectObject(currentWindowState.hdc, font);

	GetTextMetrics(currentWindowState.hdc, &tm);

	SelectObject(currentWindowState.hdc, oldFont);

	return tm.tmHeight;
}

bool UiUtils_TextFitsBoxTheme(const char text[], AbsolutePos_t pos, HFONT font) {
	SIZE textSize;

	HDC hdc = currentWindowState.memHDC;

	// Temporarily select the font into the DC
	HFONT oldFont = SelectObject(hdc, font);

	// Measure the text
	GetTextExtentPoint32A(hdc, text, strlen(text), &textSize);

	// Restore the old font
	SelectObject(hdc, oldFont);

	int boxWidth = pos.right - pos.left;
	int boxHeight = pos.bottom - pos.top;

	bool fitsWidth = textSize.cx <= boxWidth;
	bool fitsHeight = textSize.cy <= boxHeight;

	return fitsWidth && fitsHeight;
}

HFONT UiUtils_CreateFont(void) { return CreateFont(-MulDiv(10, GetDeviceCaps(currentWindowState.memHDC, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Segoe UI")); }

UiUtils_BorderEnum UiUtils_CoordinateIsOnBorderOf(int x, int y, int borderWidth, AbsolutePos_t pos) {
	// Check if on left border
	if (abs(x - pos.left) <= borderWidth) {
		if (y >= pos.top && y <= pos.bottom) {
			return LEFT;
		}
	}

	// Check if on right border
	if (abs(x - pos.right) <= borderWidth) {
		if (y >= pos.top && y <= pos.bottom) {
			return RIGHT;
		}
	}

	// Check if on bottom border
	if (abs(y - pos.bottom) <= borderWidth) {
		if (x >= pos.left && x <= pos.right) {
			return BOTTOM;
		}
	}

	// Check if on top border
	if (abs(y - pos.top) <= borderWidth) {
		if (x >= pos.left && x <= pos.right) {
			return TOP;
		}
	}

	return BORDER_NONE;
}

AbsolutePos_t getPosToContainer(const AbsolutePos_t *parentAbsPos, CommonPos_t widgetRelPos) {
	AbsolutePos_t absPos;

	int parentWidth = parentAbsPos->right - parentAbsPos->left;
	int parentHeight = parentAbsPos->bottom - parentAbsPos->top;

	absPos.left = parentAbsPos->left + (int)(widgetRelPos.left * parentWidth);
	absPos.right = parentAbsPos->left + (int)(widgetRelPos.right * parentWidth);
	absPos.top = parentAbsPos->top + (int)(widgetRelPos.top * parentHeight);
	absPos.bottom = parentAbsPos->top + (int)(widgetRelPos.bottom * parentHeight);

	return absPos;
}

bool UiUtils_WidgetFitsInContainer(AbsolutePos_t widget, AbsolutePos_t container) { return widget.left >= container.left && widget.top >= container.top && widget.right <= container.right && widget.bottom <= container.bottom; }

HFONT getFontForRect(HDC hdc, char text[], RECT box) {
	int boxWidth = box.right - box.left;
	int boxHeight = box.bottom - box.top;

	int fontSize = boxHeight;
	HFONT hFont = NULL;
	SIZE size;

	// Binary search for best font size
	int minSize = 4, maxSize = boxHeight;
	while (minSize <= maxSize) {
		fontSize = (minSize + maxSize) / 2;

		if (hFont)
			DeleteObject(hFont);
		hFont = CreateFontA(-fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
		GetTextExtentPoint32A(hdc, text, strlen(text), &size);
		SelectObject(hdc, oldFont);

		if (size.cx > boxWidth || size.cy > boxHeight)
			maxSize = fontSize - 1;
		else
			minSize = fontSize + 1;
	}

	// Use the best fitting font size found
	if (hFont)
		DeleteObject(hFont);
	hFont = CreateFontA(-maxSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

	return hFont;
}