#ifndef WIDGET_H
#define WIDGET_H

#include "common.h"

typedef struct BaseWidget BaseWidget_t;

typedef enum {
	WIDGET_TYPE_BUTTON,
	WIDGET_TYPE_INPUT,
	WIDGET_TYPE_CHECKBOX,
	WIDGET_TYPE_SLIDER,
	WIDGET_TYPE_TEXT_DUMP,
} WidgetType_t;

typedef struct BaseWidget {
	void (*drawHandler)(BaseWidget_t *);
	CommonPos_t initPos;
	void (*onClick)(BaseWidget_t *, int, int);
	void (*onHover)(BaseWidget_t *);
	void (*onHoverEnd)(BaseWidget_t *);
	WidgetType_t type;
	AbsolutePos_t * parentPos;
	AbsolutePos_t pos;
	bool hidden;
} BaseWidget_t;

typedef struct WidgetNode WidgetNode_t;

typedef struct WidgetNode {
	BaseWidget_t *widget;
	WidgetNode_t *nextWidgetNode;
} WidgetNode_t;

typedef struct WidgetList {
	WidgetNode_t *headWidget;
} WidgetList_t;

void addWidget(DynamicArray_t * array, BaseWidget_t *widget);
void renderWidgetList(DynamicArray_t * array);
void updatePosToContainerList(AbsolutePos_t containerPos, DynamicArray_t * array);
BaseWidget_t *widgetClicked(int x, int y, DynamicArray_t * array);

#endif