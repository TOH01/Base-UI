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
	COLORREF color;
    CommonPos_t pos;
    CommonPos_t initPos;
	union {
		struct {
			int width;
            COLORREF border;
		} rectangle;
		struct {
			char * text;
			HFONT font;
            UINT formatFlags;
		} label;
		struct {
			int width;
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
Drawable_t *drawable_initRectangle(COLORREF border, COLORREF fill, CommonPos_t pos, int width);
Drawable_t *drawable_initLabel(COLORREF color, UINT formatFlags, char * text, CommonPos_t pos, HFONT font);
Drawable_t *drawable_initLine(COLORREF color, CommonPos_t pos, int width);
void drawable_updatePosToContainerList(CommonPos_t containerPos, DrawableList_t *list);

#endif