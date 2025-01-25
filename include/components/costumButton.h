#ifndef CUSTOM_BUTTON_H
#define COSTUM_BUTTON_H

#include "common.h"

typedef struct ButtonWidget {
    CommonPos_t pos;
    void * onClick;
} buttonWidget_t;

#endif