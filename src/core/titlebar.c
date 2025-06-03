#define _WIN32_WINNT 0x0A00

#include "UiUtils.h"
#include "config.h"
#include "titlbar.h"
#include <stdbool.h>

#ifdef CUSTOM_TITLE_BAR

activeHover active = BORDER_NONE;

int win32_dpi_scale(int value, UINT dpi) { return (int)((float)value * dpi / 96); }

RECT win32_titlebar_rect(HWND handle) {
	SIZE title_bar_size = {0};
	const int top_and_bottom_borders = 2;
	HTHEME theme = OpenThemeData(handle, L"WINDOW");
	UINT dpi = GetDpiForWindow(handle);
	GetThemePartSize(theme, NULL, WP_CAPTION, CS_ACTIVE, NULL, TS_TRUE, &title_bar_size);
	CloseThemeData(theme);

	int height = win32_dpi_scale(title_bar_size.cy, dpi) + top_and_bottom_borders;

	RECT rect;
	GetClientRect(handle, &rect);
	rect.bottom = rect.top + height;
	return rect;
}

bool win32_window_is_maximized(HWND handle) {
	WINDOWPLACEMENT placement = {0};
	placement.length = sizeof(WINDOWPLACEMENT);
	if (GetWindowPlacement(handle, &placement)) {
		return placement.showCmd == SW_SHOWMAXIMIZED;
	}
	return false;
}

int getTitleBarHeight(HWND handle) {

	RECT titlebar = win32_titlebar_rect(handle);

	return (titlebar.bottom);
}

void drawTitlebar(HDC hdc, PAINTSTRUCT ps) {

	HBRUSH title_bar_brush = CreateSolidBrush(currentWindowState.activeTheme.titlebar.color);
	RECT title_bar_rect = win32_titlebar_rect(currentWindowState.hwnd);

	FillRect(hdc, &title_bar_rect, title_bar_brush);

	DeleteObject(title_bar_brush);

	RECT titlebar = win32_titlebar_rect(currentWindowState.hwnd);
	int height = getTitleBarHeight(currentWindowState.hwnd);

	// draw hover effect

	COLORREF hoverColor;
	RECT hoverRect;

	if (active) {

		RECT hoverRect = {titlebar.right - (active * height), titlebar.top, titlebar.right - ((active - 1) * height), titlebar.bottom};

		switch (active) {
		case CLOSE:
			hoverColor = currentWindowState.activeTheme.close.hover;
			break;
		case RESTORE:
			hoverColor = currentWindowState.activeTheme.restore.hover;
			break;
		case MINIMIZE:
			hoverColor = currentWindowState.activeTheme.minimize.hover;
			break;
		}

		HBRUSH hoverBrush = CreateSolidBrush(hoverColor);
		FillRect(hdc, &hoverRect, hoverBrush);
		DeleteObject(hoverBrush);
	}

	// draw close

	HPEN iconPenClose = CreatePen(PS_SOLID, 2, currentWindowState.activeTheme.close.ICON);
	SelectObject(currentWindowState.memHDC, iconPenClose);

	RECT closeRect = {titlebar.right - (CLOSE * height), titlebar.top, titlebar.right - ((CLOSE - 1) * height), titlebar.bottom};
	int closeWidth = closeRect.right - closeRect.left;
	int closeHeight = closeRect.bottom - closeRect.top;

	int pad = closeWidth * 0.3;
	MoveToEx(currentWindowState.memHDC, closeRect.left + pad, closeRect.top + pad, NULL);
	LineTo(currentWindowState.memHDC, closeRect.right - pad, closeRect.bottom - pad);

	MoveToEx(currentWindowState.memHDC, closeRect.right - pad, closeRect.top + pad, NULL);
	LineTo(currentWindowState.memHDC, closeRect.left + pad, closeRect.bottom - pad);

	// draw restore
	HPEN iconPenRestore = CreatePen(PS_SOLID, 2, currentWindowState.activeTheme.restore.ICON);
	SelectObject(currentWindowState.memHDC, iconPenRestore);
	HBRUSH backgroundBrush = CreateSolidBrush(currentWindowState.activeTheme.titlebar.color);
	SelectObject(currentWindowState.memHDC, backgroundBrush);

	RECT restoreRect = {titlebar.right - (RESTORE * height), titlebar.top, titlebar.right - ((RESTORE - 1) * height), titlebar.bottom};
	int widthRestore = restoreRect.right - restoreRect.left;
	int heightRestore = restoreRect.bottom - restoreRect.top;
	Rectangle(currentWindowState.memHDC, restoreRect.left + widthRestore * 0.2, restoreRect.top + heightRestore * 0.2, restoreRect.right - widthRestore * 0.2, restoreRect.bottom - heightRestore * 0.2);

	// draw minimize
	HPEN iconPenMinimize = CreatePen(PS_SOLID, 2, currentWindowState.activeTheme.minimize.ICON);
	SelectObject(currentWindowState.memHDC, iconPenMinimize);

	RECT minimizeRect = {titlebar.right - (MINIMIZE * height), titlebar.top, titlebar.right - ((MINIMIZE - 1) * height), titlebar.bottom};
	int minWidth = minimizeRect.right - minimizeRect.left;
	int minHeight = minimizeRect.bottom - minimizeRect.top;
	int yMid = minimizeRect.top + minHeight * 0.5;

	MoveToEx(currentWindowState.memHDC, minimizeRect.left + minWidth * 0.25, yMid, NULL);
	LineTo(currentWindowState.memHDC, minimizeRect.right - minWidth * 0.25, yMid);

	DeleteObject(iconPenClose);
	DeleteObject(iconPenRestore);
	DeleteObject(iconPenMinimize);
	DeleteObject(backgroundBrush);
}

LRESULT Titlebar_WmNCMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (!currentWindowState.mouseTrackingTitlebar) {
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = HOVER_DEFAULT;

		if (TrackMouseEvent(&tme)) {
			currentWindowState.mouseTrackingTitlebar = true;
		}
	}

	POINT cursor_point;
	GetCursorPos(&cursor_point);
	ScreenToClient(hwnd, &cursor_point);

	int height = getTitleBarHeight(currentWindowState.hwnd);
	RECT titlebar = win32_titlebar_rect(currentWindowState.hwnd);

	RECT closeRect = {titlebar.right - (CLOSE * height), titlebar.top, titlebar.right - ((CLOSE - 1) * height), titlebar.bottom};
	RECT restoreRect = {titlebar.right - (RESTORE * height), titlebar.top, titlebar.right - ((RESTORE - 1) * height), titlebar.bottom};
	RECT minimizeRect = {titlebar.right - (MINIMIZE * height), titlebar.top, titlebar.right - ((MINIMIZE - 1) * height), titlebar.bottom};

	int newActive = BORDER_NONE;

	if (PtInRect(&closeRect, cursor_point)) {
		newActive = CLOSE;
	} else if (PtInRect(&minimizeRect, cursor_point)) {
		newActive = MINIMIZE;
	} else if (PtInRect(&restoreRect, cursor_point)) {
		newActive = RESTORE;
	}

	if (newActive != active) {
		active = newActive;
		InvalidateRect(currentWindowState.hwnd, &titlebar, FALSE);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Titlebar_WmMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (active != BORDER_NONE) {
		active = BORDER_NONE;
		RECT titlebar = win32_titlebar_rect(hwnd);
		InvalidateRect(currentWindowState.hwnd, &titlebar, FALSE);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Titlebar_WmNCLButtonDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (active != BORDER_NONE) {
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT MenuUi_WmNCLButtonUp(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (active) {
	case CLOSE:
		PostMessageW(hwnd, WM_CLOSE, 0, 0);
		break;
	case MINIMIZE:
		ShowWindow(hwnd, SW_MINIMIZE);
		break;
	case RESTORE:
		int mode = win32_window_is_maximized(hwnd) ? SW_NORMAL : SW_MAXIMIZE;
		ShowWindow(hwnd, mode);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Titlebar_MouseLeaveCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	active = BORDER_NONE;

	currentWindowState.mouseTrackingTitlebar = false;

	InvalidateRect(hwnd, NULL, FALSE);
}

void initTitlebar(void) {
	WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEMOVE, &Titlebar_WmMouseMove);
	WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_NCMOUSELEAVE, &Titlebar_MouseLeaveCallback);
}

#endif