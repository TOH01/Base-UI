#include "drawable.h"
#include "UiUtils.h"
#include <stdio.h>

Drawable_t *drawable_initRectangle(COLORREF border, COLORREF fill, CommonPos_t pos, int width) {
	Drawable_t *drawable = (Drawable_t *)calloc(1, sizeof(Drawable_t));
	drawable->type = DRAWABLE_RECTANGLE;
	drawable->color = fill;
	drawable->rectangle.width = width;
	drawable->rectangle.border = border;
	drawable->pos = pos;
	drawable->initPos = pos;

	return drawable;
}

Drawable_t *drawable_initLabel(COLORREF color, UINT formatFlags, char * text, CommonPos_t pos, HFONT font) {
	Drawable_t *drawable = (Drawable_t *)calloc(1, sizeof(Drawable_t));
	drawable->type = DRAWABLE_LABEL;
	drawable->color = color;
    drawable->label.font = font;
    drawable->label.text = text;
    drawable->label.formatFlags = formatFlags;
	drawable->pos = pos;
	drawable->initPos = pos;

	return drawable;
}

Drawable_t *drawable_initLine(COLORREF color, CommonPos_t pos, int width) {
	Drawable_t *drawable = (Drawable_t *)calloc(1, sizeof(Drawable_t));
	drawable->type = DRAWABLE_LINE;
	drawable->type = width;
	drawable->pos = pos;
	drawable->initPos = pos;

	return drawable;
}

void drawable_draw(Drawable_t *drawable) {

	switch (drawable->type) {
	case DRAWABLE_LINE:

		break;
	case DRAWABLE_RECTANGLE:
		UiUtils_DrawColoredRectangle(drawable->pos, drawable->color, drawable->rectangle.border, drawable->rectangle.width);
		break;
	case DRAWABLE_LABEL:
		if (UiUtils_TextFitsBoxTheme(drawable->label.text, drawable->pos, drawable->label.font)) {
			UiUtils_DrawTextTheme(drawable->pos, drawable->label.text, drawable->label.formatFlags, drawable->label.font, drawable->color);
		}
#ifdef DEBUG
		else{
			printf("Drawable text too large for label");
		}
#endif
		break;

	default:
		break;
	}
}

DrawableList_t *initDrawableList(void) {
	DrawableList_t *list = (DrawableList_t *)calloc(1, sizeof(DrawableList_t));
	return list;
}

void addDrawable(DrawableList_t *list, Drawable_t *drawable) {
	if (!list) {
		return;
	}

	if (list->headDrawable == NULL) {
		list->headDrawable = (DrawableNode_t *)calloc(1, sizeof(DrawableNode_t));
		list->headDrawable->drawable = drawable;
		return;
	}

	DrawableNode_t *currentNode = list->headDrawable;

	while (currentNode->nextDrawableNode != NULL) {
		currentNode = currentNode->nextDrawableNode;
	}

	DrawableNode_t *newNode = (DrawableNode_t *)calloc(1, sizeof(DrawableNode_t));
	newNode->drawable = drawable;

	currentNode->nextDrawableNode = newNode;
}

void drawable_drawAll(DrawableList_t * list){
	if (!list || !list->headDrawable) {
		return;
	}

	DrawableNode_t *node = list->headDrawable;

	drawable_draw(node->drawable);

	while (node->nextDrawableNode != NULL) {
		node = node->nextDrawableNode;
		drawable_draw(node->drawable);
	}
}

void drawable_updatePosToContainerList(CommonPos_t containerPos, DrawableList_t *list) {

	if (!list || !list->headDrawable) {
		return;
	}

	DrawableNode_t *node = list->headDrawable;

	node->drawable->pos = getPosToContainer(containerPos, node->drawable->initPos);

	while (node->nextDrawableNode != NULL) {
		node = node->nextDrawableNode;
		node->drawable->pos = getPosToContainer(containerPos, node->drawable->initPos);
	}
}