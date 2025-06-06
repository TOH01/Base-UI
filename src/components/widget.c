#include "widget.h"
#include "UiUtils.h"
#include "common.h"

void addWidget(DynamicArray_t *array, BaseWidget_t *widget) {
	if (!array) {
		return;
	}

	DynamicArray_Add(array, widget);
}

void renderWidgetList(DynamicArray_t *array) {

	if (!array) {
		return;
	}

	BaseWidget_t *widget = NULL;

	for (int i = 0; i < array->size; i++) {
		widget = DynamicArray_get(array, i);

		if (widget != NULL && !widget->hidden) {
			widget->drawHandler(widget);
		}
	}
}

void updatePosToContainerList(DynamicArray_t *array) {

	if (!array) {
		return;
	}

	BaseWidget_t *widget = NULL;

	for (int i = 0; i < array->size; i++) {
		widget = DynamicArray_get(array, i);

		if (widget != NULL) {

			if (widget->posType == POS_TYPE_REL) {
				widget->pos = getPosToContainer(widget->parentPos, widget->initPos);
			}
			else if (widget->posType == POS_TYPE_ABS) {
				widget->pos = getPosToContainerAbsolute(widget->parentPos, widget->initPosAbs);
			}
		}
	}
}

BaseWidget_t *widgetClicked(int x, int y, DynamicArray_t *array) {

	if (!array) {
		return NULL;
	}

	BaseWidget_t *widget = NULL;

	for (int i = 0; i < array->size; i++) {
		widget = DynamicArray_get(array, i);

		if (widget != NULL && UiUtils_CoordinateIsInsideOf(x, y, widget->pos)) {
			return widget;
		}
	}

	return NULL;
}
