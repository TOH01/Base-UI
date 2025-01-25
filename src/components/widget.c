#include "widget.h"
#include "common.h"

WidgetList_t * initWidgetList(void){
    WidgetList_t * list = (WidgetList_t *) calloc(1, sizeof(WidgetList_t));
    return list;
}

void addWidget(WidgetList_t * list, BaseWidget_t * widget){
    if (!list){
        return;
    }

    if (list->headWidget == NULL){
        list->headWidget->widget = widget;
        return;
    }

    WidgetNode_t * currentNode = list->headWidget;

    while (currentNode->nextWidgetNode != NULL){
        currentNode = currentNode->nextWidgetNode;
    }

    WidgetNode_t * newNode = (WidgetNode_t *) calloc(1, sizeof(WidgetNode_t));
    newNode->widget = widget;

    currentNode->nextWidgetNode = newNode;

}

void renderWidgetList(WidgetList_t * list){
    
    if (!list || !list->headWidget){
        return;
    }
    
    WidgetNode_t * node = list->headWidget;

    node->widget->drawHandler();

    while(node->nextWidgetNode != NULL){
        node = node->nextWidgetNode;
        node->widget->drawHandler();
    }
}