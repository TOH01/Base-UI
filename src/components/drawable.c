#include "drawable.h"
#include "UiUtils.h"
#include <stdio.h>

Drawable_t *drawable_initRectangle(CommonPos_t pos, rectangleTheme_t *theme) {
	Drawable_t *drawable = (Drawable_t *)calloc(1, sizeof(Drawable_t));
	drawable->type = DRAWABLE_RECTANGLE;

	drawable->rectangle.theme = theme;

	drawable->pos = pos;
	drawable->initPos = pos;

	return drawable;
}

Drawable_t *drawable_initLabel(CommonPos_t pos, char *text, labelTheme_t *theme) {
	Drawable_t *drawable = (Drawable_t *)calloc(1, sizeof(Drawable_t));
	drawable->type = DRAWABLE_LABEL;

	drawable->label.theme = theme;

	drawable->label.text = text;

	drawable->pos = pos;
	drawable->initPos = pos;

	return drawable;
}

Drawable_t *drawable_initLine(CommonPos_t pos, lineTheme_t *theme) {
	Drawable_t *drawable = (Drawable_t *)calloc(1, sizeof(Drawable_t));
	drawable->type = DRAWABLE_LINE;
	drawable->line.theme = theme;
	drawable->pos = pos;
	drawable->initPos = pos;

	return drawable;
}

void drawable_draw(Drawable_t *drawable) {

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

void drawable_drawAll(DrawableList_t *list) {
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