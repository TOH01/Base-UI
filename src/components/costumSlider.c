#include "UiUtils.h"
#include "common.h"
#include "customSlider.h"
#include "widget.h"
#include <assert.h>
#include <stdio.h>

sliderWidget_t *activeSlider = NULL;
bool sliderHandlersRegistered = false;

CommonPos_t getPointPos(sliderWidget_t *slider) {
	CommonPos_t pointPos = slider->baseWidget.pos;

	float barWidth = slider->baseWidget.pos.right - slider->baseWidget.pos.left;
	float barHeight = slider->baseWidget.pos.bottom - slider->baseWidget.pos.top;

	float pointWidth = barWidth / (float)(slider->range);

	float pointX = slider->baseWidget.pos.left + ((*slider->value) * (barWidth - pointWidth)) / (slider->range - 1);

	pointPos.left = pointX;
	pointPos.right = pointX + pointWidth;

	pointPos.top = slider->baseWidget.pos.top + (barHeight * slider->theme->thumbSpacingTop);
	pointPos.bottom = slider->baseWidget.pos.top + (barHeight * (1 - slider->theme->thumbSpacingBottom));

	return pointPos;
}

CommonPos_t getBarPos(sliderWidget_t *slider) {
	CommonPos_t barPos = slider->baseWidget.pos;

	float barWidth = slider->baseWidget.pos.right - slider->baseWidget.pos.left;
	float barHeight = slider->baseWidget.pos.bottom - slider->baseWidget.pos.top;

	barPos.top = slider->baseWidget.pos.top + (barHeight * slider->theme->barSpacingTop);
	barPos.bottom = slider->baseWidget.pos.top + (barHeight * (1 - slider->theme->barSpacingBottom));

	return barPos;
}

static void drawSlider(BaseWidget_t *baseWidget) {
	assert(baseWidget->type == WIDGET_TYPE_SLIDER);
	sliderWidget_t *slider = (sliderWidget_t *)baseWidget;

	CommonPos_t pointPos = getPointPos(slider);
	CommonPos_t barPos = getBarPos(slider);

	COLORREF barColor = slider->selected == BAR ? slider->theme->bar.hover : slider->theme->bar.fill;
	COLORREF pointColor = slider->selected == THUMB ? slider->theme->thumb.hover : slider->theme->thumb.fill;

	UiUtils_DrawColoredRectangle(barPos, barColor, slider->theme->bar.border, slider->theme->barBorderWidht);
	UiUtils_DrawColoredRectangle(pointPos, pointColor, slider->theme->thumb.border, slider->theme->thumbBorderWidth);
}

static void onClickSlider(BaseWidget_t *baseWidget, int x, int y) {
	assert(baseWidget->type == WIDGET_TYPE_SLIDER);
	sliderWidget_t *slider = (sliderWidget_t *)baseWidget;

	float width = (slider->baseWidget.pos.right - slider->baseWidget.pos.left);

	for (int i = 0; i < slider->range; i++) {
		CommonPos_t pos = baseWidget->pos;
		pos.left = (baseWidget->pos.left + ((width / slider->range) * i));
		pos.right = baseWidget->pos.left + ((width / slider->range) * (i + 1));

		if (UiUtils_CoordinateIsInsideOf(x, y, pos)) {
			*(slider->value) = i;
		}
	}

	activeSlider = slider;
}

static void onHoverSlider(BaseWidget_t *base) {
	assert(base->type == WIDGET_TYPE_SLIDER);
	sliderWidget_t *slider = (sliderWidget_t *)base;

	if (!slider->beingHovered) {
		slider->beingHovered = 1;
		InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
	}

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(currentWindowState.hwnd, &mousePos);

	if (slider->beingHovered) {
		if (UiUtils_CoordinateIsInsideOf(mousePos.x, mousePos.y, getPointPos(slider))) {
			slider->selected = THUMB;
			InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
		} else if (UiUtils_CoordinateIsInsideOf(mousePos.x, mousePos.y, getBarPos(slider))) {
			slider->selected = BAR;
			InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
		} else {
			slider->selected = NONE;
		}
	}
}

static void onHoverEndSlider(BaseWidget_t *base) {
	assert(base->type == WIDGET_TYPE_SLIDER);
	sliderWidget_t *slider = (sliderWidget_t *)base;
	slider->beingHovered = 0;
	slider->selected = NONE;
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

LRESULT mouseMoveSlider(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (activeSlider) {

		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(hwnd, &mousePos);

		if (UiUtils_CoordinateIsInsideOf(mousePos.x, mousePos.y, activeSlider->baseWidget.pos)) {
			float width = (activeSlider->baseWidget.pos.right - activeSlider->baseWidget.pos.left);

			for (int i = 0; i < activeSlider->range; i++) {
				CommonPos_t pos = activeSlider->baseWidget.pos;
				pos.left = (activeSlider->baseWidget.pos.left + ((width / activeSlider->range) * i));
				pos.right = activeSlider->baseWidget.pos.left + ((width / activeSlider->range) * (i + 1));

				if (UiUtils_CoordinateIsInsideOf(mousePos.x, mousePos.y, pos)) {
					*(activeSlider->value) = i;
				}
			}

			RECT sliderRect = UiUtils_CommonPosToRect(activeSlider->baseWidget.pos);

			InvalidateRect(currentWindowState.hwnd, &sliderRect, FALSE);
		}
	}
}

LRESULT mouseReleaseSlider(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (activeSlider) {
		activeSlider = NULL;
	}
}

sliderWidget_t *customSlider_initSlider(CommonPos_t pos, int *value, int range) {
	sliderWidget_t *slider = (sliderWidget_t *)calloc(1, sizeof(sliderWidget_t));

	slider->baseWidget.pos = pos;
	slider->baseWidget.initPos = pos;

	slider->baseWidget.drawHandler = &drawSlider;
	slider->baseWidget.onClick = &onClickSlider;
	slider->baseWidget.onHover = &onHoverSlider;
	slider->baseWidget.onHoverEnd = &onHoverEndSlider;

	slider->range = range;
	slider->value = value;

	slider->baseWidget.type = WIDGET_TYPE_SLIDER;

	slider->theme = &currentWindowState.activeTheme.slider;

	if (!sliderHandlersRegistered) {
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEMOVE, (void *)&mouseMoveSlider);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_LBUTTONUP, (void *)&mouseReleaseSlider);
		sliderHandlersRegistered = true;
	}

	return slider;
}