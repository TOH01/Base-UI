#include "WmParamHashTable.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void HandlerListAdd(HandlerList_t* list, MessageHandler_t handler) {
    if (!list) {
        printf("Cant add Handler to list, because list does not exist\n");
        return;
    }

    HandlerNode_t* currentNode = list->firstHandlerNode;

    if (currentNode == NULL) {
        HandlerNode_t* newNode = (HandlerNode_t*)calloc(1, sizeof(HandlerNode_t));
        newNode->handler = handler;
        list->firstHandlerNode = newNode;
        return;
    }

    while (currentNode->nextHandlerNode != NULL) {
        currentNode = currentNode->nextHandlerNode;
    }

    HandlerNode_t* newNode = (HandlerNode_t*)calloc(1, sizeof(HandlerNode_t));

    currentNode->nextHandlerNode = newNode;

    newNode->handler = handler;
    newNode->nextHandlerNode = NULL;
}

HandlerList_t* HandlerListInit(void) {
    HandlerList_t* list = (HandlerList_t*)calloc(1, sizeof(HandlerList_t));

    if (list == NULL) {
        printf("Error Initializing Hander List\n");
        return NULL;
    }

    return list;
}

MessageHandler_t HandlerListGetFirstHandler(HandlerList_t* list) {
    if (!list) {
        printf("Cannot get first Handler, because list is null\n");
        return NULL;
    }

    return list->firstHandlerNode->handler;
}

int getHash(int WmParamKey) { return (WmParamKey % MAX_WM_PARAM_AMOUNT); }

WmParamHandlerTable_t* WmParamHandlerTable_Init(void) {
    WmParamHandlerTable_t* handlerTable =
        (WmParamHandlerTable_t*)calloc(1, sizeof(WmParamHandlerTable_t));

    for (int i = 0; i < MAX_WM_PARAM_AMOUNT; i++) {
        handlerTable->content[i].WmParamKey = WM_PARAM_TABLE_UNUSED_ID;
    }

    if (!handlerTable) {
        printf("Error initializing HandlerTable\n");
        return NULL;
    }

    return handlerTable;
}

int WmParamHanderTable_Insert(WmParamHandlerTable_t* hashtable, int WmParamKey,
                              MessageHandler_t handler) {
    if (!hashtable) {
        printf("Cannot Insert, because Hashtable does not exist.");
        return -1;
    }

    if (hashtable->size >= 128) {
        printf("Error WmParamHashTable Overflow\n");
        return -1;
    }

    int idx = getHash(WmParamKey);

    while (hashtable->content[idx].WmParamKey != WM_PARAM_TABLE_UNUSED_ID) {
        if (hashtable->content[idx].WmParamKey == WmParamKey) {
            HandlerListAdd(hashtable->content[idx].WmParamHandler, handler);
            return 0;
        }

        idx = (idx + 1) % MAX_WM_PARAM_AMOUNT;
    }

    hashtable->content[idx].WmParamKey = WmParamKey;

    if (!hashtable->content[idx].WmParamHandler) {
        hashtable->content[idx].WmParamHandler = HandlerListInit();
    }

    HandlerListAdd(hashtable->content[idx].WmParamHandler, handler);

    hashtable->size++;

    return 0;
}

HandlerList_t* WmParamHandlerTable_GetHandlerList(WmParamHandlerTable_t* hashtable,
                                                  int WmParamKey) {
    if (!hashtable) {
        return NULL;
    }

    int idx = getHash(WmParamKey);
    int counter = 0;

    while (hashtable->content[idx].WmParamKey != WmParamKey) {
        idx = (idx + 1) % MAX_WM_PARAM_AMOUNT;
        counter++;

        if (counter > (MAX_WM_PARAM_AMOUNT - 1)) {
            return NULL;
        }

        if (hashtable->content[idx].WmParamKey == WM_PARAM_TABLE_UNUSED_ID) {
            return NULL;
        }
    }

    return hashtable->content[idx].WmParamHandler;
}

void WmParamHandlerTable_Destroy(WmParamHandlerTable_t* hashtable) { free(hashtable); }

bool WmParamHandlerTable_IdHasHandler(WmParamHandlerTable_t* hashtable, int msg) {
    HandlerList_t* handlerList = WmParamHandlerTable_GetHandlerList(hashtable, msg);

    if (!handlerList) {
        return false;
    }

    MessageHandler_t handler = HandlerListGetFirstHandler(handlerList);

    if (handler) {
        return true;
    }

    return false;
}

void WmParamHandlerTable_CallHandlersOfId(WmParamHandlerTable_t* hashtable, HWND hwnd, int id,
                                          WPARAM wparam, LPARAM lparam) {
    if (!hashtable) {
        return;
    }

    HandlerList_t* handlers = WmParamHandlerTable_GetHandlerList(hashtable, id);

    if (handlers == NULL) {
        return;
    }

    HandlerNode_t* currentNode = handlers->firstHandlerNode;

    while (currentNode != NULL) {
        currentNode->handler(hwnd, id, wparam, lparam);
        currentNode = currentNode->nextHandlerNode;
    }
}