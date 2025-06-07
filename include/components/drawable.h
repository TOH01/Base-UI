#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "common.h"

typedef enum DrawableType {
	DRAWABLE_RECTANGLE,
	DRAWABLE_LABEL,
	DRAWABLE_LINE,
	DRAWABLE_IMG,
	DRAWABLE_3_SLICE_IMG_RECT,
} DrawableType_t;


typedef struct Drawable {
	DrawableType_t type;
    AbsolutePos_t * parentPos;
	AbsolutePos_t pos;
	bool hidden;
    CommonPos_t initPos;
	AbsolutePos_t initPosAbs;
	posType_t posType;
	union {
		struct {
			rectangleTheme_t * theme;
		} rectangle;
		struct {
			const char * text;
			labelTheme_t * theme;
		} label;
		struct {
			lineTheme_t * theme;
		} line;
		struct {
			HICON icon;
			bool iconLoaded;
			int iconID;
		} img;
		struct {
			HICON icon[3];
			bool iconsLoades;
			int iconID[3];
		} slice3img;
	};

} Drawable_t;

void drawable_drawAll(DynamicArray_t * array);
void addDrawable(DynamicArray_t * array, Drawable_t *drawable);
Drawable_t *drawable_initRectangle(CommonPos_t pos, rectangleTheme_t * theme);
Drawable_t *drawable_initLabel(CommonPos_t pos, const char * text, labelTheme_t * theme);
Drawable_t *drawable_initLine(CommonPos_t pos, lineTheme_t * theme);
Drawable_t *drawable_initImg(CommonPos_t pos, int ID);
Drawable_t *drawable_init3SliceImgRectange(CommonPos_t pos, int ID_LEFT, int ID_CENTER, int ID_RIGHT);
void drawable_updatePosToContainerList(DynamicArray_t * array);


#endif