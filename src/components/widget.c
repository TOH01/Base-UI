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
			else if(widget->posType == POS_TYPE_ANCHOR){
				widget->pos = getPosToContainerAbsolute(widget->parentPos, widget->initPosAbs);
				int width = widget->pos.right - widget->pos.left;
				switch (widget->anchor)
				{
				case WIDGET_ANCHOR_CENTER:
					int containerCenter = widget->parentPos->left + ((widget->parentPos->right - widget->parentPos->left) / 2);
					widget->pos.left = containerCenter - (int) (width / 2);
					widget->pos.right = widget->pos.left + width;
					break;
				case WIDGET_ANCHOR_LEFT:
					widget->pos.left = widget->parentPos->left + widget->anchorOffset;
					widget->pos.right = widget->pos.left + width;
					break;
				case WIDGET_ANCHOR_RIGHT:
					widget->pos.right = widget->parentPos->right - widget->anchorOffset;
					widget->pos.left = widget->pos.right - width;
					break;
				default:
					break;
				}
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
