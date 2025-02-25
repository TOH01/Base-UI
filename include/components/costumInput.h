#ifndef COSTUM_INPUT_H
#define COSTUM_INPUT_H

#include "common.h"
#include "widget.h"

#define INPUT_MAX_STRING_SIZE 256

typedef struct InputWidget {
    BaseWidget_t baseWidget;
    char buffer[INPUT_MAX_STRING_SIZE];
} inputWidget_t;


inputWidget_t * costumInput_initInput(CommonPos_t pos);
void costumInput_clearInput(inputWidget_t * input);

#endif