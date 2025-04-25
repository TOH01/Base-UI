#ifndef CONTAINER_H
#define CONTAINER_H

#include "common.h"
#include "menu.h"
#include "widget.h"
#include <stdbool.h>

#define containerPos_t CommonPos_t

typedef struct container_t {
	containerPos_t pos;
	int borderWitdh;
	WidgetList_t *widgetList;
	containerTheme_t *theme;
} container_t;

typedef struct movingContainer_t {
	int action;
	container_t *container;
	CommonPos_t startPos;
	int mouseStartX;
	int mouseStartY;
} movingContainer_t;

#define CONTAINER_MOVE_ACTION 5

container_t *initContainer(containerPos_t pos, WmParamHandlerTable_t *handlerTable);
void containerAddWidget(container_t *container, BaseWidget_t *widget);
container_t *windowAddContainer(CommonPos_t pos);

#endif