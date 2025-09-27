#include "UiUtils.h"
#include "assert.h"
#include "customInput.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool handlerInit = 0;
inputWidget_t *activeInput = NULL;

static void drawInput(BaseWidget_t *baseWidget) {
	assert(baseWidget->type == WIDGET_TYPE_INPUT);
	inputWidget_t *input = (inputWidget_t *)baseWidget;

	if (input->theme != NULL) {
		COLORREF fill = input->beingHovered ? input->theme->color.hover : input->theme->color.fill;
		COLORREF border = (input == activeInput) ? input->theme->active.border : input->theme->color.border;

		UiUtils_DrawColoredRectangle(baseWidget->pos, fill, border, input->theme->borderWidth);

		if (input->buffer && strlen(input->buffer) >= 1) {
			if (UiUtils_TextFitsBoxTheme(input->buffer, baseWidget->pos, input->theme->inputText.font)) {
				UiUtils_DrawTextTheme(baseWidget->pos, input->buffer, input->theme->inputText.formatFlags, input->theme->inputText.font, input->theme->inputText.color);
			} else {
				int lastElementIdx = (int)strlen(input->buffer) - 1;
				while (lastElementIdx >= 0 && UiUtils_TextFitsBoxTheme(&input->buffer[lastElementIdx], baseWidget->pos, input->theme->inputText.font)) {
					lastElementIdx--;
				}
				if (lastElementIdx >= 0) {
					UiUtils_DrawTextTheme(baseWidget->pos, &input->buffer[lastElementIdx], input->theme->inputText.formatFlags, input->theme->inputText.font, input->theme->inputText.color);
				}
#ifdef DEBUG
				else {
					printf("text cannot fit input\n");
				}
#endif
			}
		} else {
			// empty input: nothing to draw
		}
	}
#ifdef DEBUG
	else {
		printf("input missing theme\n");
	}
#endif
}

static void onHoverInput(BaseWidget_t *base) {
	assert(base->type == WIDGET_TYPE_INPUT);
	inputWidget_t *input = (inputWidget_t *)base;

	if (!input->beingHovered) {
		input->beingHovered = 1;
		InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
	}
}

static void onHoverEndInput(BaseWidget_t *base) {
	assert(base->type == WIDGET_TYPE_INPUT);
	inputWidget_t *input = (inputWidget_t *)base;
	input->beingHovered = 0;
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

static LRESULT keystoreCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)hwnd;
	(void)msg;
	(void)lParam;

	if (activeInput && activeInput->buffer) {
		int buffLen = (int)strlen(activeInput->buffer);

		if (wParam == VK_ESCAPE) {
			activeInput = NULL;
			return 1;
		}

		if (wParam == VK_DELETE || wParam == VK_BACK) {
			if (buffLen >= 1) {
				activeInput->buffer[buffLen - 1] = '\0';
				if (activeInput->onDataChange != NULL) {
					activeInput->onDataChange();
				}
				InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
			}
			return 1;
		}

		if (buffLen < activeInput->max_size - 1 && wParam >= 32 && wParam <= 126) {
			activeInput->buffer[buffLen] = (char)wParam;
			activeInput->buffer[buffLen + 1] = '\0';
			if (activeInput->onDataChange != NULL) {
				activeInput->onDataChange();
			}
			InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
		}
	}

	return 0;
}

static void onClickInput(BaseWidget_t *baseWidget, int x, int y, ClickType_t clickType) {
	assert(baseWidget->type == WIDGET_TYPE_INPUT);
	(void)x;
	(void)y;
	(void)clickType;
	activeInput = (inputWidget_t *)baseWidget;
}

static void freeInput(BaseWidget_t *baseWidget) {
	assert(baseWidget->type == WIDGET_TYPE_INPUT);
	inputWidget_t *input = (inputWidget_t *)baseWidget;
	free(input);
}

inputWidget_t *customInput_initInput(CommonPos_t pos, char *buffer) {
	inputWidget_t *input = (inputWidget_t *)calloc(1, sizeof(inputWidget_t));

	if (!handlerInit) {
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_KEYDOWN, &keystoreCallback);
		handlerInit = 1;
	}

	input->baseWidget.initPos = pos;
	input->baseWidget.type = WIDGET_TYPE_INPUT;

	input->buffer = buffer;

	input->theme = &currentWindowState.activeTheme.input;

	input->baseWidget.drawHandler = &drawInput;
	input->baseWidget.onClick = &onClickInput;
	input->baseWidget.onHover = &onHoverInput;
	input->baseWidget.onHoverEnd = &onHoverEndInput;
	input->baseWidget.destroy = &freeInput;

	return input;
}

void customInput_clearInput(inputWidget_t *input) {
	if (input->buffer)
		input->buffer[0] = '\0';
}