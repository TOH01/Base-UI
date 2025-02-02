#ifndef CUSTOM_BUTTON_H
#define COSTUM_BUTTON_H

#include "common.h"
#include "widget.h"

#define BUTTON_MAX_NAME_LENGTH 30

typedef struct ButtonWidget {
    BaseWidget_t baseWidget;
    int id;
    void ( * onClickUserCallback) (int);
    char name[BUTTON_MAX_NAME_LENGTH];
} buttonWidget_t;

buttonWidget_t * initButton(CommonPos_t pos, void (*onClickUserCallback)(int), int id);
void buttonSetText(buttonWidget_t * button, char name[BUTTON_MAX_NAME_LENGTH]);

#endif