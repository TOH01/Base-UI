#include "titlbar.h"
#include <stdbool.h>

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

void drawTitlebar(HDC hdc, PAINTSTRUCT ps) {

	COLORREF title_bar_color = RGB(150, 200, 180);
	HBRUSH title_bar_brush = CreateSolidBrush(title_bar_color);
	RECT title_bar_rect = win32_titlebar_rect(currentWindowState.hwnd);

	FillRect(hdc, &title_bar_rect, title_bar_brush);
}
