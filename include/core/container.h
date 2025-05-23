#ifndef CONTAINER_H
#define CONTAINER_H

#include "common.h"
#include "menu.h"
#include "widget.h"
#include "drawable.h"
#include <stdbool.h>

#define containerPos_t AbsolutePos_t

typedef struct container_t {
	AbsolutePos_t absPos;
	DynamicArray_t * widgetList;
	DynamicArray_t * drawableList;
	containerTheme_t *theme;
	bool visible;
	bool movable;
	bool resizable;
} container_t;

typedef struct movingContainer_t {
	int action;
	container_t *container;
	AbsolutePos_t startPos;
	int mouseStartX;
	int mouseStartY;
} movingContainer_t;

#define CONTAINER_MOVE_ACTION 5

container_t *initContainer(containerPos_t pos);
void containerAddWidget(container_t *container, BaseWidget_t *widget);
container_t *windowAddContainer(AbsolutePos_t pos);
void containerAddDrawable(container_t * container, Drawable_t * drawable);
void initRootContainer();
void rootContainerAddDrawable(Drawable_t *drawable);
void rootContainerAddWidget(BaseWidget_t *widget);

#endif