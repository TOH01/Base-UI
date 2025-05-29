#include "UiUtils.h"
#include "common.h"
#include "customCheckbox.h"
#include "widget.h"
#include <assert.h>
#include <stdio.h>

static void drawCheckbox(BaseWidget_t *baseWidget) {
	assert(baseWidget->type == WIDGET_TYPE_CHECKBOX);
	checkboxWidget_t *checkbox = (checkboxWidget_t *)baseWidget;

	if (checkbox->theme) {
		UiUtils_DrawColoredRectangle(baseWidget->pos, checkbox->theme->outer.fill, checkbox->theme->outer.border, checkbox->theme->borderWidth);

		if (*(checkbox->value) || checkbox->beingHovered) {

			AbsolutePos_t inner = baseWidget->pos;

			float height = inner.bottom - inner.top;
			float width = inner.right - inner.left;

			inner.bottom = inner.bottom - (height * checkbox->theme->spacing);
			inner.top = inner.top + (height * checkbox->theme->spacing);
			inner.right = inner.right - (width * checkbox->theme->spacing);
			inner.left = inner.left + (width * checkbox->theme->spacing);

			COLORREF fill = checkbox->beingHovered ? checkbox->theme->inner.hover : checkbox->theme->inner.fill;

			UiUtils_DrawColoredRectangle(inner, fill, checkbox->theme->inner.border, checkbox->theme->borderWidth);
		}
	}
#ifdef DEBUG
	else {
		printf("checkbox missing theme\n");
	}
#endif
}

static void onHoverCheckbox(BaseWidget_t *base) {
	assert(base->type == WIDGET_TYPE_CHECKBOX);
	checkboxWidget_t * checkbox = (checkboxWidget_t *)base;

	if (!checkbox->beingHovered) {
		checkbox->beingHovered = 1;
		InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
	}
}

static void onHoverEndCheckbox(BaseWidget_t *base) {
	assert(base->type == WIDGET_TYPE_CHECKBOX);
	checkboxWidget_t * checkbox = (checkboxWidget_t *)base;
	checkbox->beingHovered = 0;
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

static void onClickCheckbox(BaseWidget_t *baseWidget, int x, int y, ClickType_t clickType) {
	assert(baseWidget->type == WIDGET_TYPE_CHECKBOX);
	checkboxWidget_t *checkbox = (checkboxWidget_t *)baseWidget;
	*(checkbox->value) = !*(checkbox->value);
}

checkboxWidget_t *customCheckbox_initCheckbox(CommonPos_t pos, bool *value) {
	checkboxWidget_t *checkbox = (checkboxWidget_t *)calloc(1, sizeof(checkboxWidget_t));
	
	checkbox->baseWidget.initPos = pos;

	checkbox->baseWidget.type = WIDGET_TYPE_CHECKBOX;

	checkbox->baseWidget.drawHandler = &drawCheckbox;
	checkbox->baseWidget.onClick = &onClickCheckbox;
	checkbox->baseWidget.onHover = &onHoverCheckbox;
	checkbox->baseWidget.onHoverEnd = &onHoverEndCheckbox;

	checkbox->theme = &currentWindowState.activeTheme.checkbox;

	checkbox->value = value;
}