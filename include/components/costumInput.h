#ifndef COSTUM_INPUT_H
#define COSTUM_INPUT_H

#include "common.h"
#include "widget.h"

typedef struct InputWidget {
    BaseWidget_t baseWidget;
    char buffer[256];
} inputWidget_t;


inputWidget_t * initInput(CommonPos_t pos);

#endif