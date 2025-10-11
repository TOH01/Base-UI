#include "WmParamHashTable.h"

#include <stdbool.h>
#include <stdlib.h>

#include "debug.h"

void WmParamHashTable_Add(HandlerList_t* list, MessageHandler_t handler) {
    if (list) {
        HandlerNode_t* currentNode = list->firstHandlerNode;

        if (currentNode == NULL) {
            HandlerNode_t* newNode = (HandlerNode_t*)calloc(1, sizeof(HandlerNode_t));
            newNode->handler = handler;
            list->firstHandlerNode = newNode;
        } else {
            while (currentNode->nextHandlerNode != NULL) {
                currentNode = currentNode->nextHandlerNode;
            }

            HandlerNode_t* newNode = (HandlerNode_t*)calloc(1, sizeof(HandlerNode_t));

            currentNode->nextHandlerNode = newNode;

            newNode->handler = handler;
            newNode->nextHandlerNode = NULL;
        }

    } else {
        DEBUG_ERROR("WmParamHashTable_Add: handler list not found");
    }
}

HandlerList_t* WmParamHashTable_Init(void) {
    HandlerList_t* list = (HandlerList_t*)calloc(1, sizeof(HandlerList_t));

    if (list == NULL) {
        DEBUG_ERROR("WmParamHashTable_Init: error initalizing handler table");
    }

    return list;
}

MessageHandler_t WmParamHashTable_GetFirstHandler(HandlerList_t* list) {
    MessageHandler_t handler = NULL;

    if (!list) {
        DEBUG_ERROR("WmParamHashTable_GetFirstHandler: handler table is null");
    }
    if (!list->firstHandlerNode) {
        DEBUG_ERROR("WmParamHashTable_GetFirstHandler: handler table has no nodes yet");
    } else {
        handler = list->firstHandlerNode->handler;
    }

    return handler;
}

int getHash(int WmParamKey) { return (WmParamKey % MAX_WM_PARAM_AMOUNT); }

WmParamHandlerTable_t* WmParamHandlerTable_Init(void) {
    WmParamHandlerTable_t* handlerTable = (WmParamHandlerTable_t*)calloc(1, sizeof(WmParamHandlerTable_t));

    for (int i = 0; i < MAX_WM_PARAM_AMOUNT; i++) {
        handlerTable->content[i].WmParamKey = WM_PARAM_TABLE_UNUSED_ID;
    }

    if (handlerTable == NULL) {
        DEBUG_ERROR("WmParamHashTable_GetFirstHandler: handler table is null");
    }

    return handlerTable;
}

int WmParamHanderTable_Insert(WmParamHandlerTable_t* hashtable, int WmParamKey, MessageHandler_t handler) {
    int status = -1;

    if (!hashtable) {
        DEBUG_ERROR("WmParamHanderTable_Insert: handler table is null");
    } else if (hashtable->size >= MAX_WM_PARAM_AMOUNT) {
        DEBUG_ERROR("WmParamHanderTable_Insert: handler table is full");
    } else {
        int idx = getHash(WmParamKey);

        while (hashtable->content[idx].WmParamKey != WM_PARAM_TABLE_UNUSED_ID) {
            if (hashtable->content[idx].WmParamKey == WmParamKey) {
                WmParamHashTable_Add(hashtable->content[idx].WmParamHandler, handler);
                return 0;
            }

            idx = (idx + 1) % MAX_WM_PARAM_AMOUNT;
        }

        hashtable->content[idx].WmParamKey = WmParamKey;

        if (!hashtable->content[idx].WmParamHandler) {
            hashtable->content[idx].WmParamHandler = WmParamHashTable_Init();
        }

        WmParamHashTable_Add(hashtable->content[idx].WmParamHandler, handler);

        hashtable->size++;

        status = 0;
    }

    return status;
}

HandlerList_t* WmParamHandlerTable_GetHandlerList(WmParamHandlerTable_t* hashtable, int WmParamKey) {
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

    MessageHandler_t handler = WmParamHashTable_GetFirstHandler(handlerList);

    if (handler) {
        return true;
    }

    return false;
}

void WmParamHandlerTable_CallHandlersOfId(WmParamHandlerTable_t* hashtable, HWND hwnd, int id, WPARAM wparam, LPARAM lparam) {
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