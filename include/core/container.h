#ifndef CONTAINER_H
#define CONTAINER_H

#include "common.h"
#include "menu.h"
#include "widget.h"
#include "drawable.h"
#include <stdbool.h>

#define containerPos_t AbsolutePos_t

typedef enum LayoutType {
	LAYOUT_NONE,
	LAYOUT_CONTAINER,
	LAYOUT_BORDER_LEFT,
	LAYOUT_BORDER_RIGHT,
	LAYOUT_BORDER_TOP,
	LAYOUT_BORDER_BOTTOM,
} LayoutType_t;


typedef struct Layout {
	LayoutType_t top;
	LayoutType_t left;
	LayoutType_t right;
	LayoutType_t bottom;

	int offsetTop;
	int offsetLeft;
	int offsetRight;
	int offsetBottom;
} Layout_t;

typedef struct container_t {
	AbsolutePos_t absPos;
	DynamicArray_t * widgetList;
	DynamicArray_t * drawableList;
	containerTheme_t *theme;
	bool visible;
	bool movable;
	bool resizable;
	bool fixed;				// only set true, if this container is not dependent on other containers for sizing
	bool fixedWidgets;
	bool disableRectRender;
	Layout_t layout;
	bool grid;
	int cols;
	int rows;
	bool limitGrid;
	int cellMinWidth;
	int cellMaxWidth;
	int cellMaxHeight;
	int cellMinHeight;
	BaseWidget_t ** gridPositions;
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
void initRootContainer(void);
void rootContainerAddDrawable(Drawable_t *drawable);
void rootContainerAddWidget(BaseWidget_t *widget);
void setContainerFixed(container_t * container);
void containerAddWidgetAbsolute(container_t * container, BaseWidget_t * widget, AbsolutePos_t pos);
void containerAddDrawableAbsolute(container_t * container, Drawable_t *drawable, AbsolutePos_t pos);
void containerAddWidgetAnchored(container_t *container, BaseWidget_t *widget, AbsolutePos_t pos, AnchorPos_t anchor, int anchorOffset);
void updateContainersLayoutPos(void);
void updateWidgetVisibility(void);
container_t *windowAddGridContainer(AbsolutePos_t pos, int rows, int cols); 
void addWidgetToGridContainer(container_t * container, BaseWidget_t * widget, int row, int col);
void destroyContainerContent(container_t * container);

#endif