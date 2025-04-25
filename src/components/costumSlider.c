#include "UiUtils.h"
#include "common.h"
#include "customSlider.h"
#include "widget.h"
#include <assert.h>
#include <stdio.h>

sliderWidget_t *activeSlider = NULL;
bool sliderHandlersRegistered = false;

static void drawSlider(BaseWidget_t *baseWidget) {
	assert(baseWidget->type == WIDGET_TYPE_SLIDER);
	sliderWidget_t *slider = (sliderWidget_t *)baseWidget;

	CommonPos_t pointPos = baseWidget->pos;
	CommonPos_t barPos = baseWidget->pos;

	float barWidth = baseWidget->pos.right - baseWidget->pos.left;
	float barHeight = baseWidget->pos.bottom - baseWidget->pos.top;

	barPos.top = baseWidget->pos.top + (barHeight * slider->theme->barSpacingTop);
	barPos.bottom = baseWidget->pos.top + (barHeight * (1 - slider->theme->barSpacingBottom));

	float pointWidth = barWidth / (float)(slider->range);

	float pointX = baseWidget->pos.left + ((*slider->value) * (barWidth - pointWidth)) / (slider->range - 1);

	pointPos.left = pointX;
	pointPos.right = pointX + pointWidth;

	pointPos.top = baseWidget->pos.top + (barHeight * slider->theme->thumbSpacingTop);
	pointPos.bottom = baseWidget->pos.top + (barHeight * (1 - slider->theme->thumbSpacingBottom));

	UiUtils_DrawColoredRectangle(barPos, slider->theme->bar.fill, slider->theme->bar.border, slider->theme->barBorderWidht);
	UiUtils_DrawColoredRectangle(pointPos, slider->theme->thumb.fill, slider->theme->thumb.border, slider->theme->thumbBorderWidth);
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

LRESULT mouseMoveSlider(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (activeSlider) {

		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(hwnd, &mousePos);

		float width = (activeSlider->baseWidget.pos.right - activeSlider->baseWidget.pos.left);

		for (int i = 0; i < activeSlider->range; i++) {
			CommonPos_t pos = activeSlider->baseWidget.pos;
			pos.left = (activeSlider->baseWidget.pos.left + ((width / activeSlider->range) * i));
			pos.right = activeSlider->baseWidget.pos.left + ((width / activeSlider->range) * (i + 1));

			if (UiUtils_CoordinateIsInsideOf(mousePos.x, mousePos.y, pos)) {
				*(activeSlider->value) = i;
			}
		}

		InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
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