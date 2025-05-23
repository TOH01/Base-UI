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
    AbsolutePos_t * parentPos;
	AbsolutePos_t pos;
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

void drawable_drawAll(DynamicArray_t * array);
void addDrawable(DynamicArray_t * array, Drawable_t *drawable);
Drawable_t *drawable_initRectangle(CommonPos_t pos, rectangleTheme_t * theme);
Drawable_t *drawable_initLabel(CommonPos_t pos, char * text, labelTheme_t * theme);
Drawable_t *drawable_initLine(CommonPos_t pos, lineTheme_t * theme);
void drawable_updatePosToContainerList(AbsolutePos_t containerPos, DynamicArray_t * array);

#endif