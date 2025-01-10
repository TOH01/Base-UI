#ifndef CONTAINER_H
#define CONTAINER_H

#include "common.h"
#include "MenuUi.h"

#define containerPos_t CommonPos_t

typedef struct container_t {
    containerPos_t pos;
} container_t;

container_t * initContainer(containerPos_t pos, WmParamHandlerTable_t * handlerTable);

#endif