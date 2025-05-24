#include "drawable.h"
#include "UiUtils.h"
#include <stdio.h>

Drawable_t *drawable_initRectangle(CommonPos_t pos, rectangleTheme_t *theme) {
	Drawable_t *drawable = (Drawable_t *)calloc(1, sizeof(Drawable_t));
	drawable->type = DRAWABLE_RECTANGLE;

	drawable->rectangle.theme = theme;


	drawable->initPos = pos;

	return drawable;
}

Drawable_t *drawable_initLabel(CommonPos_t pos, char *text, labelTheme_t *theme) {
	Drawable_t *drawable = (Drawable_t *)calloc(1, sizeof(Drawable_t));
	drawable->type = DRAWABLE_LABEL;

	drawable->label.theme = theme;

	drawable->label.text = text;


	drawable->initPos = pos;

	return drawable;
}

Drawable_t *drawable_initLine(CommonPos_t pos, lineTheme_t *theme) {
	Drawable_t *drawable = (Drawable_t *)calloc(1, sizeof(Drawable_t));
	drawable->type = DRAWABLE_LINE;
	drawable->line.theme = theme;

	drawable->initPos = pos;

	return drawable;
}

void drawable_draw(Drawable_t *drawable) {

	if(drawable->hidden){
		return;
	}

	switch (drawable->type) {
	case DRAWABLE_LINE:

		lineTheme_t *theme = drawable->line.theme;

		UiUtils_DrawLineRelative(drawable->pos, theme->color.fill, theme->width);

		break;
	case DRAWABLE_RECTANGLE:

		rectangleTheme_t *themeRectangle = drawable->rectangle.theme;

		UiUtils_DrawColoredRectangle(drawable->pos, themeRectangle->color.fill, themeRectangle->color.border, themeRectangle->borderWidth);
		break;
	case DRAWABLE_LABEL:

		labelTheme_t *themeLabel = drawable->label.theme;

		if (UiUtils_TextFitsBoxTheme(drawable->label.text, drawable->pos, themeLabel->text.font)) {
			UiUtils_DrawTextTheme(drawable->pos, drawable->label.text, themeLabel->text.formatFlags, themeLabel->text.font, themeLabel->text.color);
		}
#ifdef DEBUG
		else {
			printf("Drawable text too large for label");
		}
#endif
		break;

	default:
		break;
	}
}

void addDrawable(DynamicArray_t * array, Drawable_t *drawable) {
	if (!array || !drawable) {
		return;
	}

	DynamicArray_Add(array, drawable);
	
}

void drawable_drawAll(DynamicArray_t *array) {
	if (!array) {
		return;
	}

	Drawable_t * drawable = NULL;

	for (int i = 0; i < array->size; i++)
	{
		
		drawable = (Drawable_t *) DynamicArray_get(array, i);

		drawable_draw(drawable);
	}
	
}

void drawable_updatePosToContainerList(AbsolutePos_t containerPos, DynamicArray_t * array) {

	if (!array) {
		return;
	}

	Drawable_t * drawable = NULL;

	for (int i = 0; i < array->size; i++)
	{
		drawable = (Drawable_t *) DynamicArray_get(array, i);

		drawable->pos = getPosToContainer(drawable->parentPos, drawable->initPos);
	}
	
}