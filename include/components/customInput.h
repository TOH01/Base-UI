#ifndef COSTUM_INPUT_H
#define COSTUM_INPUT_H

#include "common.h"
#include "widget.h"

#define INPUT_MAX_STRING_SIZE 256
#define INPUT_MAX_DEFAULT_STRING_SIZE 20

typedef enum {
    INPUT_MODE_TEXT,
    INPUT_MODE_NUMERIC
} InputMode_t;

typedef struct InputWidget {
	BaseWidget_t baseWidget;
	InputMode_t type;
	void (*onDataChange)(void);
	char buffer[INPUT_MAX_STRING_SIZE];
	int * value;
	bool beingHovered;
	char defaultText[INPUT_MAX_DEFAULT_STRING_SIZE];
	InputTheme_t *theme;
} inputWidget_t;

inputWidget_t *customInput_initInput(CommonPos_t pos);
inputWidget_t *customInput_initInputNumeric(CommonPos_t pos, int * value);
void customInput_clearInput(inputWidget_t *input);
void setDefaultText(inputWidget_t * input, char * defaultText);
void endActiveInput(void);
void setOnActiveInputEndCbk(void * end);

#endif