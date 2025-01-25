#ifndef WIDGET_H
#define WIDGET_H

enum WidgetType {
    button,
    input,
};

typedef struct BaseWidget {
    void (*drawHandler)(void);
    int type;
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

#endif