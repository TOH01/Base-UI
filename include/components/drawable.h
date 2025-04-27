#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "common.h"

typedef enum DrawableType {
	DRAWABLE_RECTANGLE,
	DRAWABLE_LABEL,
	DRAWABLE_LINE,
} DrawableType_t;


typedef struct Drawable {
	DrawableType_t type;
    CommonPos_t pos;
    CommonPos_t initPos;
	union {
		struct {
			rectangleTheme_t * theme;
		} rectangle;
		struct {
			char * text;
			labelTheme_t * theme;
		} label;
		struct {
			lineTheme_t * theme;
		} line;
	};

} Drawable_t;

typedef struct DrawableNode DrawableNode_t;

typedef struct DrawableNode {
	Drawable_t *drawable;
	DrawableNode_t *nextDrawableNode;
} DrawableNode_t;

typedef struct drawableList {
	DrawableNode_t *headDrawable;
} DrawableList_t;

void drawable_drawAll(DrawableList_t * list);
void addDrawable(DrawableList_t *list, Drawable_t *drawable);
DrawableList_t *initDrawableList(void);
Drawable_t *drawable_initRectangle(CommonPos_t pos, rectangleTheme_t * theme);
Drawable_t *drawable_initLabel(CommonPos_t pos, char * text, labelTheme_t * theme);
Drawable_t *drawable_initLine(CommonPos_t pos, lineTheme_t * theme);
void drawable_updatePosToContainerList(CommonPos_t containerPos, DrawableList_t *list);

#endif