#ifndef CONTAINER_H
#define CONTAINER_H

#include "common.h"
#include "MenuUi.h"
#include <stdbool.h>
#include "widget.h"

#define containerPos_t CommonPos_t

typedef struct container_t {
    containerPos_t pos;
    int borderWitdh;
    WidgetList_t * widgetList;
} container_t;

typedef struct movingContainer_t {
    int action;
    container_t * container;
    CommonPos_t startPos;
    int mouseStartX;
    int mouseStartY;
} movingContainer_t;

#define CONTAINER_MOVE_ACTION 5

container_t * initContainer(containerPos_t pos, WmParamHandlerTable_t * handlerTable);

#endif