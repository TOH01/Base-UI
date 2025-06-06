#include "coreWindowState.h"
#include "config.h"
#include "container.h"
#include "titlbar.h"

WindowState_t currentWindowState = {
    .width = CONFIG_INIT_WINDOW_WIDTH,
    .height = CONFIG_INIT_WINDOW_HEIGTH,
    .handlerTable = NULL,
    .hPen = NULL,
    .ps = NULL,
    .hdc = NULL,
    .activeTheme = DARKMODE_THEME(),
};

LRESULT CoreWindowState_WmCreateHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)msg;
	(void)wParam;
	(void)lParam;

	currentWindowState.hwnd = hwnd;
#ifdef CUSTOM_TITLE_BAR
	SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);

	currentWindowState.titlbarHeight = getTitleBarHeight(hwnd);

	for (int i = 1; i < currentWindowState.containers->size; i++) {
		container_t *container = (container_t *)DynamicArray_get(currentWindowState.containers, i);

		container->absPos.top += currentWindowState.titlbarHeight;
		container->absPos.bottom += currentWindowState.titlbarHeight;

		updatePosToContainerList(container->widgetList);
		drawable_updatePosToContainerList(container->drawableList);
	} 

#endif
	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}

LRESULT CoreWindowState_WmSizeHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)msg;
	(void)wParam;

	container_t *container = (container_t *)DynamicArray_get(currentWindowState.containers, 0);

	container->absPos.bottom = HIWORD(lParam);
	container->absPos.right = LOWORD(lParam);

	InvalidateRect(hwnd, NULL, FALSE);

	if (LOWORD(lParam) > CONFIG_MIN_WINDOW_WIDTH) {
		currentWindowState.width = LOWORD(lParam);
	}
	if (HIWORD(lParam) > CONFIG_MIN_WINDOW_Height) {
		currentWindowState.height = HIWORD(lParam);
	}

	return 0;
}