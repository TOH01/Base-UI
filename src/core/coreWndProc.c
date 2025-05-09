#define _WIN32_WINNT 0x0A00

#include "coreWndProc.h"
#include "Common.h"
#include "WmParamHashTable.h"
#include "menu.h"
#include "titlbar.h"
#include <stdio.h>

int layoutInitialzed = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		WmParamHandlerTable_Destroy(currentWindowState.handlerTable);
		PostQuitMessage(0);
		break;
#ifdef CUSTOM_TITLE_BAR
	case WM_NCCALCSIZE: {
		if (!wParam)
			return DefWindowProc(hwnd, msg, wParam, lParam);
		UINT dpi = GetDpiForWindow(hwnd);

		int frame_x = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
		int frame_y = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
		int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

		NCCALCSIZE_PARAMS *params = (NCCALCSIZE_PARAMS *)lParam;
		RECT *requested_client_rect = params->rgrc;

		requested_client_rect->right -= frame_x + padding;
		requested_client_rect->left += frame_x + padding;
		requested_client_rect->bottom -= frame_y + padding;

		if (win32_window_is_maximized(hwnd)) {
			requested_client_rect->top += padding;
		}

		return 0;
	}
	case WM_NCHITTEST: {
		// Let the default procedure handle resizing areas
		LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
		switch (hit) {
		case HTNOWHERE:
		case HTRIGHT:
		case HTLEFT:
		case HTTOPLEFT:
		case HTTOP:
		case HTTOPRIGHT:
		case HTBOTTOMRIGHT:
		case HTBOTTOM:
		case HTBOTTOMLEFT: {
			return hit;
		}
		}

		// Looks like adjustment happening in NCCALCSIZE is messing with the detection
		// of the top hit area so manually fixing that.
		UINT dpi = GetDpiForWindow(hwnd);
		int frame_y = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
		int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
		POINT cursor_point = {0};
		cursor_point.x = GET_X_PARAM(lParam);
		cursor_point.y = GET_Y_PARAM(lParam);
		ScreenToClient(hwnd, &cursor_point);
		if (cursor_point.y > 0 && cursor_point.y < frame_y + padding) {
			return HTTOP;
		}

		// Since we are drawing our own caption, this needs to be a custom test
		if (cursor_point.y < win32_titlebar_rect(hwnd).bottom) {
			return HTCAPTION;
		}

		return HTCLIENT;
	}
	case WM_NCLBUTTONDOWN:
		return Titlebar_WmNCLButtonDown(hwnd, msg, wParam, lParam);
	case WM_NCLBUTTONUP:
		return MenuUi_WmNCLButtonUp(hwnd, msg, wParam, lParam);
	case WM_NCMOUSEMOVE:
		return Titlebar_WmNCMouseMove(hwnd, msg, wParam, lParam);
#endif

	case WM_PAINT:

		// sometimes layout wont initalized properly for the first time, for example content area expands into titlebar
		if (!layoutInitialzed) {
			layoutInitialzed = 1;

			RECT client;
			GetClientRect(hwnd, &client);
			SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(client.right, client.bottom));
		}

		currentWindowState.hdc = BeginPaint(hwnd, &currentWindowState.ps);

		// Create and select the memory device context
		currentWindowState.memHDC = CreateCompatibleDC(currentWindowState.hdc);
		currentWindowState.memBitmap = CreateCompatibleBitmap(currentWindowState.hdc, currentWindowState.width, currentWindowState.height);
		HBITMAP hbmOld = SelectObject(currentWindowState.memHDC, currentWindowState.memBitmap);

// call draw handlers (draw to memDC)

		MenuUi_CallAllActiveHandlers(hwnd, msg, wParam, lParam);


		WmParamHandlerTable_CallHandlersOfId(currentWindowState.handlerTable, hwnd, msg, wParam, lParam);

#ifdef CUSTOM_TITLE_BAR
		drawTitlebar(currentWindowState.memHDC, currentWindowState.ps);
#endif

		// Blit to the screen
		BitBlt(currentWindowState.hdc, 0, 0, currentWindowState.width, currentWindowState.height, currentWindowState.memHDC, 0, 0, SRCCOPY);

		// Clean up memory DC and bitmap
		SelectObject(currentWindowState.memHDC, hbmOld);
		DeleteObject(currentWindowState.memBitmap);
		DeleteDC(currentWindowState.memHDC);

		// End the paint process
		EndPaint(hwnd, &currentWindowState.ps);

		break;
	default:

		MenuUi_SubmenuCommandHandler(hwnd, msg, wParam, lParam);


		if (WmParamHandlerTable_IdHasHandler(currentWindowState.handlerTable, msg)) {

			WmParamHandlerTable_CallHandlersOfId(currentWindowState.handlerTable, hwnd, msg, wParam, lParam);

		} else {
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
	return 0;
}