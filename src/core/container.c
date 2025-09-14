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

int layoutToBorderHelper(LayoutType_t type, int offset) {
	switch (type) {
	case LAYOUT_BORDER_BOTTOM:
		return currentWindowState.height + offset;
		break;
	case LAYOUT_BORDER_LEFT:
		return offset;
		break;
	case LAYOUT_BORDER_RIGHT:
		return currentWindowState.width + offset;
		break;
	case LAYOUT_BORDER_TOP:
		return offset;
		break;
	default:
		return 0;
		break;
	}
}

void updateContainersLayoutPos(void) {

	container_t *currContainer = NULL;

	for (int i = 0; i < currentWindowState.containers->size; i++) {

		currContainer = DynamicArray_get(currentWindowState.containers, i);

		if (currContainer->fixed) {
			if (currContainer->layout.right != LAYOUT_CONTAINER && currContainer->layout.right != LAYOUT_NONE) {
				currContainer->absPos.right = layoutToBorderHelper(currContainer->layout.right, currContainer->layout.offsetRight);
			}
			if (currContainer->layout.bottom != LAYOUT_CONTAINER && currContainer->layout.bottom != LAYOUT_NONE) {
				currContainer->absPos.bottom = layoutToBorderHelper(currContainer->layout.bottom, currContainer->layout.offsetBottom);
			}
			if (currContainer->layout.top != LAYOUT_CONTAINER && currContainer->layout.top != LAYOUT_NONE) {
				currContainer->absPos.top = layoutToBorderHelper(currContainer->layout.top, currContainer->layout.offsetTop);
			}
			if (currContainer->layout.left != LAYOUT_CONTAINER && currContainer->layout.left != LAYOUT_NONE) {
				currContainer->absPos.left = layoutToBorderHelper(currContainer->layout.left, currContainer->layout.offsetLeft);
			}
		}

		updatePosToContainerList(currContainer->widgetList);
		drawable_updatePosToContainerList(currContainer->drawableList);
	}
}

void updateWidgetVisibility(void) {
	int containerCount = currentWindowState.containers->size;

	for (int i = 0; i < containerCount; i++) {
		container_t *container = (container_t *)DynamicArray_get(currentWindowState.containers, i);

		if (container && container->fixedWidgets) {

			// Update widget visibility
			int widgetCount = container->widgetList->size;
			for (int j = 0; j < widgetCount; j++) {
				BaseWidget_t *widget = (BaseWidget_t *)DynamicArray_get(container->widgetList, j);
				if (widget) {
					widget->hidden = !UiUtils_WidgetFitsInContainer(widget->pos, container->absPos);
				}
			}

			// Update drawable visibility
			int drawCount = container->drawableList->size;
			for (int j = 0; j < drawCount; j++) {
				Drawable_t *drawable = (Drawable_t *)DynamicArray_get(container->drawableList, j);
				if (drawable) {
					drawable->hidden = !UiUtils_WidgetFitsInContainer(drawable->pos, container->absPos);
				}
			}
		}
	}
}

void redrawContainer(container_t *container) {
	if (!container->disableRectRender) {
		UiUtils_DrawColoredRectangle(container->absPos, container->theme->color.fill, container->theme->color.border, container->theme->borderWidth);
	}
}

void redrawContainerList(void) {

	container_t *container = NULL;

	for (int i = 0; i < currentWindowState.containers->size; i++) {

		container = (container_t *)DynamicArray_get(currentWindowState.containers, i);

		if (container && container->visible) {
			redrawContainer(DynamicArray_get(currentWindowState.containers, i));
			if (!(movingContainer.action && container == movingContainer.container)) {
				drawable_drawAll(container->drawableList);
				renderWidgetList(container->widgetList);
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

void containerListClick(int x, int y, ClickType_t clickType) {

	// reset selected input, has to be done before widget callback
	activeInput = NULL;
	activeTextDump = NULL;
	container_t *container = NULL;

	for (int i = currentWindowState.containers->size - 1; i >= 0; i--) {

		container = (container_t *)DynamicArray_get(currentWindowState.containers, i);

		if (container && container->visible) {
			if (UiUtils_CoordinateIsOnBorderOf(x, y, container->theme->borderWidth, container->absPos) && !movingContainer.action && container->resizable && clickType == CLICK_TYPE_LDOWN) {
				movingContainer.startPos = container->absPos;
				movingContainer.action = UiUtils_CoordinateIsOnBorderOf(x, y, container->theme->borderWidth, container->absPos);
				movingContainer.container = container;
				movingContainer.mouseStartX = x;
				movingContainer.mouseStartY = y;
				break;
			} else if (UiUtils_CoordinateIsInsideOf(x, y, container->absPos) && container->visible) {

				BaseWidget_t *widget = widgetClicked(x, y, container->widgetList);

				if (widget && !widget->hidden) {
					widget->onClick(widget, x, y, clickType);
					InvalidateRect(currentWindowState.hwnd, NULL, FALSE); // redraw for interactive widgets like checkboxes, which need redraw on click
					break;
				} else if (!movingContainer.action && container->movable && clickType == CLICK_TYPE_LDOWN) {
					movingContainer.startPos = container->absPos;
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
	(void)hwnd;
	(void)msg;
	(void)wParam;
	(void)lParam;
	redrawContainerList();
	return 0;
}

LRESULT resizeContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)msg;
	(void)wParam;
	(void)lParam;
	updateContainersLayoutPos();
	updateWidgetVisibility();
	InvalidateRect(hwnd, NULL, FALSE);
	return 0;
}

LRESULT LButtonDownCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)hwnd;
	(void)msg;
	(void)wParam;

	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	containerListClick(x, y, CLICK_TYPE_LDOWN);

	return 0;
}

LRESULT RButtonDownCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)hwnd;
	(void)msg;
	(void)wParam;
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	containerListClick(x, y, CLICK_TYPE_RDOWN);

	return 0;
}

LRESULT RButtonUpCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)hwnd;
	(void)msg;
	(void)wParam;
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	containerListClick(x, y, CLICK_TYPE_RUP);

	return 0;
}

LRESULT MButtonDownCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)hwnd;
	(void)msg;
	(void)wParam;
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	containerListClick(x, y, CLICK_TYPE_MDOWN);

	return 0;
}

LRESULT MButtonUpCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)hwnd;
	(void)msg;
	(void)wParam;
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	containerListClick(x, y, CLICK_TYPE_MUP);

	return 0;
}

LRESULT LButtonUpCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)msg;
	(void)wParam;
	if (movingContainer.action) {

		if (!movingContainer.container->fixedWidgets) {
			updatePosToContainerList(movingContainer.container->widgetList);
			drawable_updatePosToContainerList(movingContainer.container->drawableList);
		}

		movingContainer.action = 0;
	}

	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	containerListClick(x, y, CLICK_TYPE_LUP);

	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}

LRESULT MouseMoveCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)msg;
	(void)wParam;
	int x = LOWORD(lParam); // Horizontal position in client area
	int y = HIWORD(lParam); // Vertical position in client area

	if (!currentWindowState.mouseTrackingActive) {
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = HOVER_DEFAULT;

		if (TrackMouseEvent(&tme)) {
			currentWindowState.mouseTrackingActive = true;
		}
	}

	if (movingContainer.action) {

		int deltaX = x - movingContainer.mouseStartX;
		int deltaY = y - movingContainer.mouseStartY;

		if (movingContainer.action == CONTAINER_MOVE_ACTION) {
			movingContainer.container->absPos.left = movingContainer.startPos.left + deltaX;
			movingContainer.container->absPos.right = movingContainer.startPos.right + deltaX;
			movingContainer.container->absPos.top = movingContainer.startPos.top + deltaY;
			movingContainer.container->absPos.bottom = movingContainer.startPos.bottom + deltaY;
		} else {
			switch (movingContainer.action) {
			case TOP:
				movingContainer.container->absPos.top = movingContainer.startPos.top + deltaY;
				break;
			case BOTTOM:
				movingContainer.container->absPos.bottom = movingContainer.startPos.bottom + deltaY;
				break;
			case LEFT:
				movingContainer.container->absPos.left = movingContainer.startPos.left + deltaX;
				break;
			case RIGHT:
				movingContainer.container->absPos.right = movingContainer.startPos.right + deltaX;
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
				if (UiUtils_CoordinateIsInsideOf(x, y, container->absPos)) {
					hoverCandidate = widgetClicked(x, y, container->widgetList);
					break;
				}
			}
		}

		if (hoverCandidate != lastHoverCandidate) {
			// If there was a previous hover, end it
			if (lastHoverCandidate && lastHoverCandidate->onHoverEnd) {
				lastHoverCandidate->onHoverEnd(lastHoverCandidate);
			}

			// Start new hover
			if (hoverCandidate && !hoverCandidate->hidden) {
				hoverStartTime = GetTickCount();
				SetTimer(currentWindowState.hwnd, HOVER_TIMER_ID, HOVER_DELAY_MS, NULL);
			} else {
				KillTimer(currentWindowState.hwnd, HOVER_TIMER_ID);
			}

			lastHoverCandidate = hoverCandidate;
		}
	}

	return 0;
}

LRESULT TimerCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)wParam;
	(void)lParam;
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

	return 0;
}

LRESULT MouseLeaveCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)msg;
	(void)wParam;
	(void)lParam;
	KillTimer(hwnd, HOVER_TIMER_ID);

	if (lastHoverCandidate != NULL && lastHoverCandidate->onHoverEnd != NULL) {
		lastHoverCandidate->onHoverEnd(lastHoverCandidate);
	}

	lastHoverCandidate = NULL;
	hoverCandidate = NULL;

	currentWindowState.mouseTrackingActive = false;

	InvalidateRect(hwnd, NULL, FALSE);
	return 0;
}

container_t *initContainer(containerPos_t pos) {

	container_t *container = (container_t *)calloc(1, sizeof(container_t));

	if (!currentWindowState.handlerTable->hasContainerHandlers) {
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_PAINT, &redrawContainers);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_SIZE, &resizeContainers);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_LBUTTONDOWN, &LButtonDownCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_LBUTTONUP, &LButtonUpCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_RBUTTONDOWN, &RButtonDownCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_RBUTTONUP, &RButtonUpCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MBUTTONDOWN, &MButtonDownCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MBUTTONUP, &MButtonUpCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEMOVE, &MouseMoveCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_TIMER, &TimerCallback);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSELEAVE, &MouseLeaveCallback);

		currentWindowState.handlerTable->hasContainerHandlers = true;
	}

	container->widgetList = DynamicArray_init(10);   // TODO : better init size logic to increase performance
	container->drawableList = DynamicArray_init(10); // TODO : better init size logic to increase performance

	container->absPos = pos;

	container->theme = &currentWindowState.activeTheme.container;

	return container;
}

void containerAddWidget(container_t *container, BaseWidget_t *widget) {
	widget->parentPos = &container->absPos;
	widget->pos = getPosToContainer(widget->parentPos, widget->initPos);
	widget->posType = POS_TYPE_REL;
	addWidget(container->widgetList, widget);
}

void containerAddWidgetAbsolute(container_t *container, BaseWidget_t *widget, AbsolutePos_t pos) {
	widget->parentPos = &container->absPos;
	widget->pos = getPosToContainerAbsolute(widget->parentPos, pos);
	widget->initPosAbs = pos;
	widget->posType = POS_TYPE_ABS;
	addWidget(container->widgetList, widget);
}

void containerAddWidgetAnchored(container_t *container, BaseWidget_t *widget, AbsolutePos_t pos, AnchorPos_t anchor, int anchorOffset){
	widget->parentPos = &container->absPos;
	widget->pos = getPosToContainerAbsolute(widget->parentPos, pos);
	widget->initPosAbs = pos;
	widget->posType = POS_TYPE_ANCHOR;
	widget->anchor = anchor;
	widget->anchorOffset = anchorOffset;
	addWidget(container->widgetList, widget);
}

void containerAddDrawable(container_t *container, Drawable_t *drawable) {
	drawable->parentPos = &container->absPos;
	drawable->pos = getPosToContainer(drawable->parentPos, drawable->initPos);
	drawable->posType = POS_TYPE_REL;
	addDrawable(container->drawableList, drawable);
}

void containerAddDrawableAbsolute(container_t *container, Drawable_t *drawable, AbsolutePos_t pos) {
	drawable->parentPos = &container->absPos;
	drawable->pos = getPosToContainerAbsolute(drawable->parentPos, pos);
	drawable->initPosAbs = pos;
	drawable->posType = POS_TYPE_ABS;
	addDrawable(container->drawableList, drawable);
}

container_t *windowAddContainer(AbsolutePos_t pos) {

	container_t *container = initContainer(pos);

	container->visible = 1;

	if (currentWindowState.containers == NULL) {
		currentWindowState.containers = DynamicArray_init(50);
	}

	DynamicArray_Add(currentWindowState.containers, container);

	return container;
}

void initRootContainer(void) {

	AbsolutePos_t rootPos = {0, 0, currentWindowState.width, currentWindowState.height};

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

void setContainerFixed(container_t *container) {
	container->movable = false;
	container->resizable = false;
}
