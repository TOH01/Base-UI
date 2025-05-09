#include "container.h"
#include "WmParamHashTable.h"
#include "menu.h"
#include <assert.h>
#include <stdio.h>

movingContainer_t movingContainer;
BaseWidget_t *hoverCandidate = NULL;
BaseWidget_t *lastHoverCandidate = NULL;
DWORD hoverStartTime = 0;
#define HOVER_TIMER_ID 1
#define HOVER_DELAY_MS 5

void redrawContainer(container_t *container) { UiUtils_DrawColoredRectangle(container->pos, container->theme->color.fill, container->theme->color.border, container->theme->borderWidth); }

void redrawContainerList() {

	container_t *container = NULL;

	for (int i = 0; i < currentWindowState.containers->size; i++) {

		container = (container_t *)DynamicArray_get(currentWindowState.containers, i);

		if (container && container->visible) {
			redrawContainer(DynamicArray_get(currentWindowState.containers, i));
			if (!(movingContainer.action && container == movingContainer.container)) {
				renderWidgetList(container->widgetList);
				drawable_drawAll(container->drawableList);
			}
		}
	}
}

void moveContainerOnTop(int idx) {

	// root container has idx 0, if for whatever reason we get here, do not move it on top
	if (idx && idx < currentWindowState.containers->size - 1) {

		for (int i = idx; i < currentWindowState.containers->size - 1; i++) {
			DynamicArray_Swap(currentWindowState.containers, i, i + 1);
		}
	}
}

void containerListLButtonDown(int x, int y) {

	// reset selected input, has to be done before widget callback
	activeInput = NULL;
	activeTextDump = NULL;
	container_t *container = NULL;

	for (int i = currentWindowState.containers->size - 1; i >= 0; i--) {

		container = (container_t *)DynamicArray_get(currentWindowState.containers, i);

		if (container && container->visible) {
			if (UiUtils_CoordinateIsOnBorderOf(x, y, container->theme->borderWidth, container->pos) && !movingContainer.action && container->resizable) {
				movingContainer.startPos = container->pos;
				movingContainer.action = UiUtils_CoordinateIsOnBorderOf(x, y, container->theme->borderWidth, container->pos);
				movingContainer.container = container;
				movingContainer.mouseStartX = x;
				movingContainer.mouseStartY = y;
				break;
			} else if (UiUtils_CoordinateIsInsideOf(x, y, container->pos) && container->visible) {

				BaseWidget_t *widget = widgetClicked(x, y, container->widgetList);

				if (widget) {
					widget->onClick(widget, x, y);
					InvalidateRect(currentWindowState.hwnd, NULL, FALSE); // redraw for interactive widgets like checkboxes, which need redraw on click
					break;
				} else if (!movingContainer.action && container->movable) {
					movingContainer.startPos = container->pos;
					movingContainer.action = CONTAINER_MOVE_ACTION;
					movingContainer.container = container;
					movingContainer.mouseStartX = x;
					movingContainer.mouseStartY = y;
					moveContainerOnTop(i);
					break;
				}
			}
		}
	}
}

LRESULT redrawContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	redrawContainerList();
	return 0;
}

LRESULT resizeContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	InvalidateRect(hwnd, NULL, FALSE);
	return 0;
}

LRESULT LButtonDownCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	containerListLButtonDown(x, y);
}

LRESULT LButtonUpCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (movingContainer.action) {

		updatePosToContainerList(movingContainer.container->pos, movingContainer.container->widgetList);
		drawable_updatePosToContainerList(movingContainer.container->pos, movingContainer.container->drawableList);

		movingContainer.action = 0;
	}

	InvalidateRect(hwnd, NULL, FALSE);
}

LRESULT MouseMoveCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int x = LOWORD(lParam); // Horizontal position in client area
	int y = HIWORD(lParam); // Vertical position in client area

	if (movingContainer.action) {

		float x_relative = (float)x / (float)currentWindowState.width;
		float y_relative = (float)y / (float)currentWindowState.height;

		float start_x_relative = (float)movingContainer.mouseStartX / (float)currentWindowState.width;
		float start_y_relative = (float)movingContainer.mouseStartY / (float)currentWindowState.height;

		if (movingContainer.action == CONTAINER_MOVE_ACTION) {
			movingContainer.container->pos.left = movingContainer.startPos.left + (x_relative - start_x_relative);
			movingContainer.container->pos.top = movingContainer.startPos.top + (y_relative - start_y_relative);
			movingContainer.container->pos.right = movingContainer.startPos.right + (x_relative - start_x_relative);
			movingContainer.container->pos.bottom = movingContainer.startPos.bottom + (y_relative - start_y_relative);
		}

		else {
			switch (movingContainer.action) {
			case TOP:
				movingContainer.container->pos.top = movingContainer.startPos.top + (y_relative - start_y_relative);
				break;
			case BOTTOM:
				movingContainer.container->pos.bottom = movingContainer.startPos.bottom + (y_relative - start_y_relative);
				break;
			case LEFT:
				movingContainer.container->pos.left = movingContainer.startPos.left + (x_relative - start_x_relative);
				break;
			case RIGHT:
				movingContainer.container->pos.right = movingContainer.startPos.right + (x_relative - start_x_relative);
				break;
			default:
				break;
			}
		}

		InvalidateRect(hwnd, NULL, FALSE);

	}

	else {

		container_t *container = NULL;
		hoverCandidate = NULL;

		for (int i = currentWindowState.containers->size - 1; i >= 0; i--) {
			container = (container_t *)DynamicArray_get(currentWindowState.containers, i);

			if (container && container->visible) {
				if (UiUtils_CoordinateIsInsideOf(x, y, container->pos)) {
					hoverCandidate = widgetClicked(x, y, container->widgetList);
					break;
				}
			}
		}

		if (hoverCandidate != lastHoverCandidate) {
			if (hoverCandidate) {
				// Hovered element changed, restart timer
				hoverStartTime = GetTickCount();
				SetTimer(currentWindowState.hwnd, HOVER_TIMER_ID, HOVER_DELAY_MS, NULL);
			} else {
				// Mouse moved away, stop timer
				KillTimer(currentWindowState.hwnd, HOVER_TIMER_ID);

				if (lastHoverCandidate != NULL) {

					if (lastHoverCandidate->onHoverEnd != NULL) {
						lastHoverCandidate->onHoverEnd(lastHoverCandidate);
					} else {
						printf("Hover End callback missing");
					}

				} else {
					printf("Hover Error");
				}
			}
			lastHoverCandidate = hoverCandidate;
		}
	}
}

LRESULT CALLBACK TimerCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_TIMER && wParam == HOVER_TIMER_ID) {
		if (lastHoverCandidate) {
			if (GetTickCount() - hoverStartTime >= HOVER_DELAY_MS) {
				// Hover effect triggered
				if (hoverCandidate->onHover != NULL) {
					hoverCandidate->onHover(hoverCandidate);
				}
			}
		} else {
			// No hover candidate, stop timer
			KillTimer(hwnd, HOVER_TIMER_ID);
		}
	}
}

container_t *initContainer(containerPos_t pos) {

	container_t *container = (container_t *)calloc(1, sizeof(container_t));

	container->pos = pos;

	if (!currentWindowState.handlerTable->hasContainerHandlers) {
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_PAINT, &redrawContainers);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_SIZE, &resizeContainers);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_LBUTTONDOWN, &LButtonDownCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_LBUTTONUP, &LButtonUpCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEMOVE, &MouseMoveCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_TIMER, &TimerCallback);

		currentWindowState.handlerTable->hasContainerHandlers = true;
	}

	container->widgetList = DynamicArray_init(10);   // TODO : better init size logic to increase performance
	container->drawableList = DynamicArray_init(10); // TODO : better init size logic to increase performance

	container->theme = &currentWindowState.activeTheme.container;

	return container;
}

void containerAddWidget(container_t *container, BaseWidget_t *widget) {
	widget->pos = getPosToContainer(container->pos, widget->pos);
	addWidget(container->widgetList, widget);
}

void containerAddDrawable(container_t *container, Drawable_t *drawable) {
	drawable->pos = getPosToContainer(container->pos, drawable->pos);
	addDrawable(container->drawableList, drawable);
}

container_t *windowAddContainer(CommonPos_t pos) {

	container_t *container = initContainer(pos);

	container->visible = 1;

	if (currentWindowState.containers == NULL) {
		currentWindowState.containers = DynamicArray_init(50);
	}

	DynamicArray_Add(currentWindowState.containers, container);

	return container;
}

void initRootContainer() {

	CommonPos_t rootPos = {UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(0), UI_UTILS_PERCENT(100), UI_UTILS_PERCENT(100)};

	container_t *rootContainer = windowAddContainer(rootPos);

	container_t *check = (container_t *)DynamicArray_get(currentWindowState.containers, 0);

	assert(check == rootContainer);

	rootContainer->movable = 0;
	rootContainer->resizable = 0;
	rootContainer->theme->color.fill = currentWindowState.activeTheme.backgroundColor;
}

void rootContainerAddWidget(BaseWidget_t *widget) {
	container_t *rootContainer = (container_t *)DynamicArray_get(currentWindowState.containers, 0);
	containerAddWidget(rootContainer, widget);
}

void rootContainerAddDrawable(Drawable_t *drawable) {
	container_t *rootContainer = (container_t *)DynamicArray_get(currentWindowState.containers, 0);
	containerAddDrawable(rootContainer, drawable);
}