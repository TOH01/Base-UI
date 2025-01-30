#ifndef CUSTOM_BUTTON_H
#define COSTUM_BUTTON_H

#include "common.h"
#include "widget.h"

typedef struct ButtonWidget {
    BaseWidget_t baseWidget;
    int id;
    void ( * onClickUserCallback) (int);
} buttonWidget_t;

buttonWidget_t * initButton(CommonPos_t pos, void (*onClickUserCallback)(int), int id);

#endif