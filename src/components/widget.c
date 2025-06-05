#include "widget.h"
#include "UiUtils.h"
#include "common.h"

void addWidget(DynamicArray_t * array, BaseWidget_t *widget) {
	if (!array) {
		return;
	}

	DynamicArray_Add(array, widget);
}

void renderWidgetList(DynamicArray_t * array) {

	if (!array) {
		return;
	}

	BaseWidget_t * widget = NULL;

	for (int i = 0; i < array->size; i++)
	{
		widget = DynamicArray_get(array, i);
		
		if(widget != NULL && !widget->hidden){
			widget->drawHandler(widget);
		}
	}
}

void updatePosToContainerList(DynamicArray_t * array) {

	if (!array) {
		return;
	}

	BaseWidget_t * widget = NULL;

	for (int i = 0; i < array->size; i++)
	{
		widget = DynamicArray_get(array, i);
		
		if(widget != NULL){
			widget->pos = getPosToContainer(widget->parentPos, widget->initPos);
		}
	}

}

BaseWidget_t *widgetClicked(int x, int y, DynamicArray_t * array) {

	if (!array) {
		return NULL;
	}

	BaseWidget_t * widget = NULL;

	for (int i = 0; i < array->size; i++)
	{
		widget = DynamicArray_get(array, i);
		
		if (widget != NULL && UiUtils_CoordinateIsInsideOf(x, y, widget->pos)) {
			return widget;
		}
	}

	return NULL;
}
