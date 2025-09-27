#ifndef COSTUM_INPUT_H
#define COSTUM_INPUT_H

#include "common.h"
#include "widget.h"

typedef struct InputWidget {
	BaseWidget_t baseWidget;
	char * buffer ;
	int max_size;
	bool beingHovered;
	InputTheme_t *theme;
	void (*onDataChange)(void);
} inputWidget_t;

inputWidget_t *customInput_initInput(CommonPos_t pos, char * buffer);
void customInput_clearInput(inputWidget_t *input);

#endif