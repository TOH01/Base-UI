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
        list->headWidget = (WidgetNode_t *) calloc(1, sizeof(WidgetNode_t));
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

    node->widget->drawHandler(node->widget);

    while(node->nextWidgetNode != NULL){
        node = node->nextWidgetNode;
        node->widget->drawHandler(node->widget);
    }
}

CommonPos_t getPosToContainer(CommonPos_t containerPos, CommonPos_t widgetPos){
    CommonPos_t pos;
    
    pos.spacingLeft = containerPos.spacingLeft + ((containerPos.width - containerPos.spacingLeft) * widgetPos.spacingLeft);
    pos.width = containerPos.spacingLeft + ((containerPos.width - containerPos.spacingLeft) * widgetPos.width);

    pos.spacingTop = containerPos.spacingTop + ((containerPos.height - containerPos.spacingTop) * widgetPos.spacingTop);
    pos.height = containerPos.spacingTop + ((containerPos.height - containerPos.spacingTop) * widgetPos.height);

    return pos;
}

void updatePosToContainerList(CommonPos_t containerPos, WidgetList_t * list){
    
    if (!list || !list->headWidget){
        return;
    }
    
    WidgetNode_t * node = list->headWidget;

    node->widget->pos = getPosToContainer(containerPos, node->widget->initPos);

    while(node->nextWidgetNode != NULL){
        node = node->nextWidgetNode;
        node->widget->pos = getPosToContainer(containerPos, node->widget->initPos);
    }
}

