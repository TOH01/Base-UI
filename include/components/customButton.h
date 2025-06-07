#ifndef CUSTOM_BUTTON_H
#define COSTUM_BUTTON_H

#include "common.h"
#include "widget.h"
#include "drawable.h"

#define BUTTON_MAX_NAME_LENGTH 30

typedef struct ButtonWidget {
	BaseWidget_t baseWidget;
	int id;
	void (*onClickUserCallback)(int);
	char name[BUTTON_MAX_NAME_LENGTH];
	bool beingHovered;
	ButtonTheme_t *theme;
	HICON images[3];
} buttonWidget_t;

buttonWidget_t *customButton_initButton(CommonPos_t pos, void (*onClickUserCallback)(int), int id);
void customButton_setButtonText(buttonWidget_t *button, const char *name);
void customButton_setToIcon(buttonWidget_t * button, int iconID);
void customButton_setTo3SliceBackground(buttonWidget_t * button, int iconIDleft, int iconIDcenter, int iconIDright);

#endif