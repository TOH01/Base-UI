#include "UiUtils.h"
#include "common.h"
#include "string.h"
#include <stdio.h>
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

AbsolutePos_t getPosToContainerAbsolute(const AbsolutePos_t *parentAbsPos, AbsolutePos_t widgetAbsPos) {
	AbsolutePos_t absPos;

	absPos.left = parentAbsPos->left + widgetAbsPos.left;
	absPos.right = parentAbsPos->left + widgetAbsPos.right;
	absPos.top = parentAbsPos->top + widgetAbsPos.top;
	absPos.bottom = parentAbsPos->top + widgetAbsPos.bottom;

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

void draw3SliceHelper(AbsolutePos_t pos, HICON left, HICON center, HICON right) {
	int height = pos.bottom - pos.top;
	int width = pos.right - pos.left;

	int canDraw = (width >= height * 2);

	int fullTiles = 0;
	int remainder = 0;
	int centerStartX = pos.left + height;

	if (canDraw) {
		fullTiles = (width - 2 * height) / height;
		remainder = (width - 2 * height) % height;
	}

	// Draw full center tiles
	if (canDraw) {
		for (int i = 0; i < fullTiles; ++i) {
			int x = centerStartX + i * height;
			DrawIconEx(currentWindowState.memHDC, x, pos.top, center, height, height, 0, NULL, DI_NORMAL);
		}
	}

	// Draw partial center tile if needed (with alpha support)
	if (canDraw && remainder > 0) {
		HDC screenDC = GetDC(NULL);
		HDC iconDC = CreateCompatibleDC(screenDC);
		HBITMAP bmp = CreateCompatibleBitmap(screenDC, remainder, height);
		HBITMAP oldBmp = (HBITMAP)SelectObject(iconDC, bmp);

		// Clear background
		RECT clearRect = {0, 0, remainder, height};
		HBRUSH clearBrush = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(iconDC, &clearRect, clearBrush);
		DeleteObject(clearBrush);

		// Draw center icon into temp DC
		DrawIconEx(iconDC, 0, 0, center, height, height, 0, NULL, DI_NORMAL);

		// Alpha blend to screen
		BLENDFUNCTION blendFunc = {0};
		blendFunc.BlendOp = AC_SRC_OVER;
		blendFunc.SourceConstantAlpha = 255;
		blendFunc.AlphaFormat = AC_SRC_ALPHA;

		AlphaBlend(currentWindowState.memHDC, centerStartX + fullTiles * height, pos.top, remainder, height, iconDC, 0, 0, remainder, height, blendFunc);

		// Cleanup
		SelectObject(iconDC, oldBmp);
		DeleteObject(bmp);
		DeleteDC(iconDC);
		ReleaseDC(NULL, screenDC);
	}

	// Draw left and right caps
	if (canDraw) {
		DrawIconEx(currentWindowState.memHDC, pos.left, pos.top, left, height, height, 0, NULL, DI_NORMAL);
		DrawIconEx(currentWindowState.memHDC, pos.right - height, pos.top, right, height, height, 0, NULL, DI_NORMAL);
	}

#ifdef DEBUG
	if (!canDraw) {
		printf("3-slice bounding box too small!\n");
	}
#endif
}

// Helper function to draw a partial tile using alpha blending
static void drawPartialTile(HDC targetHDC, int x, int y, int width, int height, HICON icon, int iconWidth, int iconHeight) {
	HDC screenDC = GetDC(NULL);
	HDC iconDC = CreateCompatibleDC(screenDC);
	HBITMAP bmp = CreateCompatibleBitmap(screenDC, width, height);
	HBITMAP oldBmp = (HBITMAP)SelectObject(iconDC, bmp);

	// Clear background
	RECT clearRect = {0, 0, width, height};
	HBRUSH clearBrush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(iconDC, &clearRect, clearBrush);
	DeleteObject(clearBrush);

	// Draw icon into temp DC
	DrawIconEx(iconDC, 0, 0, icon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);

	// Alpha blend to target
	BLENDFUNCTION blendFunc = {0};
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.SourceConstantAlpha = 255;
	blendFunc.AlphaFormat = AC_SRC_ALPHA;
	AlphaBlend(targetHDC, x, y, width, height, iconDC, 0, 0, width, height, blendFunc);

	// Cleanup
	SelectObject(iconDC, oldBmp);
	DeleteObject(bmp);
	DeleteDC(iconDC);
	ReleaseDC(NULL, screenDC);
}

// Helper to draw horizontal edge (top or bottom)
static void drawHorizontalEdge(HDC hdc, int startX, int y, int centerWidth, HICON edgeIcon, int sliceWidth, int sliceHeight) {
	if (centerWidth <= 0)
		return;

	int tilesX = centerWidth / sliceWidth;
	int remainderX = centerWidth % sliceWidth;

	// Draw full tiles
	for (int i = 0; i < tilesX; i++) {
		int x = startX + i * sliceWidth;
		DrawIconEx(hdc, x, y, edgeIcon, sliceWidth, sliceHeight, 0, NULL, DI_NORMAL);
	}

	// Draw partial tile if needed
	if (remainderX > 0) {
		int x = startX + tilesX * sliceWidth;
		drawPartialTile(hdc, x, y, remainderX, sliceHeight, edgeIcon, sliceWidth, sliceHeight);
	}
}

// Helper to draw vertical edge (left or right)
static void drawVerticalEdge(HDC hdc, int x, int startY, int centerHeight, HICON edgeIcon, int sliceWidth, int sliceHeight) {
	if (centerHeight <= 0)
		return;

	int tilesY = centerHeight / sliceHeight;
	int remainderY = centerHeight % sliceHeight;

	// Draw full tiles
	for (int i = 0; i < tilesY; i++) {
		int y = startY + i * sliceHeight;
		DrawIconEx(hdc, x, y, edgeIcon, sliceWidth, sliceHeight, 0, NULL, DI_NORMAL);
	}

	// Draw partial tile if needed
	if (remainderY > 0) {
		int y = startY + tilesY * sliceHeight;
		drawPartialTile(hdc, x, y, sliceWidth, remainderY, edgeIcon, sliceWidth, sliceHeight);
	}
}

// Helper to draw center area
static void drawCenterArea(HDC hdc, int startX, int startY, int centerWidth, int centerHeight, HICON centerIcon, int sliceWidth, int sliceHeight) {
	if (centerWidth <= 0 || centerHeight <= 0)
		return;

	int tilesX = centerWidth / sliceWidth;
	int tilesY = centerHeight / sliceHeight;
	int remainderX = centerWidth % sliceWidth;
	int remainderY = centerHeight % sliceHeight;

	// Draw full center tiles
	for (int y = 0; y < tilesY; y++) {
		for (int x = 0; x < tilesX; x++) {
			int drawX = startX + x * sliceWidth;
			int drawY = startY + y * sliceHeight;
			DrawIconEx(hdc, drawX, drawY, centerIcon, sliceWidth, sliceHeight, 0, NULL, DI_NORMAL);
		}
	}

	// Draw partial tiles on right edge
	if (remainderX > 0) {
		for (int y = 0; y < tilesY; y++) {
			int drawX = startX + tilesX * sliceWidth;
			int drawY = startY + y * sliceHeight;
			drawPartialTile(hdc, drawX, drawY, remainderX, sliceHeight, centerIcon, sliceWidth, sliceHeight);
		}
	}

	// Draw partial tiles on bottom edge
	if (remainderY > 0) {
		for (int x = 0; x < tilesX; x++) {
			int drawX = startX + x * sliceWidth;
			int drawY = startY + tilesY * sliceHeight;
			drawPartialTile(hdc, drawX, drawY, sliceWidth, remainderY, centerIcon, sliceWidth, sliceHeight);
		}
	}

	// Draw corner partial tile
	if (remainderX > 0 && remainderY > 0) {
		int drawX = startX + tilesX * sliceWidth;
		int drawY = startY + tilesY * sliceHeight;
		drawPartialTile(hdc, drawX, drawY, remainderX, remainderY, centerIcon, sliceWidth, sliceHeight);
	}
}

void draw9SliceHelper(AbsolutePos_t pos, HICON topLeft, HICON topCenter, HICON topRight, HICON midLeft, HICON center, HICON midRight, HICON bottomLeft, HICON bottomCenter, HICON bottomRight, int sliceWidth, int sliceHeight) {
	int width = pos.right - pos.left;
	int height = pos.bottom - pos.top;
	int centerWidth = width - 2 * sliceWidth;
	int centerHeight = height - 2 * sliceHeight;
	HDC hdc = currentWindowState.memHDC;

	// Draw corners
	DrawIconEx(hdc, pos.left, pos.top, topLeft, sliceWidth, sliceHeight, 0, NULL, DI_NORMAL);
	DrawIconEx(hdc, pos.right - sliceWidth, pos.top, topRight, sliceWidth, sliceHeight, 0, NULL, DI_NORMAL);
	DrawIconEx(hdc, pos.left, pos.bottom - sliceHeight, bottomLeft, sliceWidth, sliceHeight, 0, NULL, DI_NORMAL);
	DrawIconEx(hdc, pos.right - sliceWidth, pos.bottom - sliceHeight, bottomRight, sliceWidth, sliceHeight, 0, NULL, DI_NORMAL);

	// Draw edges
	drawHorizontalEdge(hdc, pos.left + sliceWidth, pos.top, centerWidth, topCenter, sliceWidth, sliceHeight);
	drawHorizontalEdge(hdc, pos.left + sliceWidth, pos.bottom - sliceHeight, centerWidth, bottomCenter, sliceWidth, sliceHeight);
	drawVerticalEdge(hdc, pos.left, pos.top + sliceHeight, centerHeight, midLeft, sliceWidth, sliceHeight);
	drawVerticalEdge(hdc, pos.right - sliceWidth, pos.top + sliceHeight, centerHeight, midRight, sliceWidth, sliceHeight);

	// Draw center
	drawCenterArea(hdc, pos.left + sliceWidth, pos.top + sliceHeight, centerWidth, centerHeight, center, sliceWidth, sliceHeight);
}