#ifndef WIDGET_H
#define WIDGET_H

#include "common.h"

enum WidgetType {
    button,
    input,
};

typedef struct BaseWidget BaseWidget_t;

typedef struct BaseWidget {
    void (*drawHandler)(BaseWidget_t *);
    CommonPos_t initPos;
    int type;
    CommonPos_t pos;
} BaseWidget_t;

typedef struct WidgetNode WidgetNode_t;

typedef struct WidgetNode {
    BaseWidget_t * widget;
    WidgetNode_t * nextWidgetNode;
} WidgetNode_t;


typedef struct WidgetList {
    WidgetNode_t * headWidget;
} WidgetList_t;

void addWidget(WidgetList_t * list, BaseWidget_t * widget);
WidgetList_t * initWidgetList(void);
void renderWidgetList(WidgetList_t * list);
CommonPos_t getPosToContainer(CommonPos_t containerPos, CommonPos_t widgetPos);
void updatePosToContainerList(CommonPos_t containerPos, WidgetList_t * list);

#endif