#include <stdbool.h>
#include <stdlib.h>

#include "WmParamHashTable.h"
#include "debug.h"
#include "windows.h"

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
    } else if (!list->firstHandlerNode) {
        DEBUG_ERROR("WmParamHashTable_GetFirstHandler: handler table has no nodes yet");
    } else {
        handler = list->firstHandlerNode->handler;
    }

    return handler;
}

int getHash(int WmParamKey) { return (WmParamKey % MAX_WM_PARAM_AMOUNT); }

WmParamTable_t* WmParamTable_Init(void) {
    WmParamTable_t* handlerTable = (WmParamTable_t*)calloc(1, sizeof(WmParamTable_t));

    for (int i = 0; i < MAX_WM_PARAM_AMOUNT; i++) {
        handlerTable->entries[i].WmParamKey = WM_PARAM_TABLE_UNUSED_ID;
    }

    if (handlerTable == NULL) {
        DEBUG_ERROR("WmParamHashTable_GetFirstHandler: handler table is null");
    }

    return handlerTable;
}

int WmParamTable_Insert(WmParamTable_t* hashtable, int WmParamKey, MessageHandler_t handler) {
    int status = -1;

    if (!hashtable) {
        DEBUG_ERROR("WmParamTable_Insert: handler table is null");
    } else if (hashtable->size >= MAX_WM_PARAM_AMOUNT) {
        DEBUG_ERROR("WmParamTable_Insert: handler table is full");
    } else {
        int idx = getHash(WmParamKey);

        while (hashtable->entries[idx].WmParamKey != WM_PARAM_TABLE_UNUSED_ID) {
            if (hashtable->entries[idx].WmParamKey == WmParamKey) {
                WmParamHashTable_Add(hashtable->entries[idx].handlerList, handler);
                return 0;
            }

            idx = (idx + 1) % MAX_WM_PARAM_AMOUNT;
        }

        hashtable->entries[idx].WmParamKey = WmParamKey;

        if (!hashtable->entries[idx].handlerList) {
            hashtable->entries[idx].handlerList = WmParamHashTable_Init();
        }

        WmParamHashTable_Add(hashtable->entries[idx].handlerList, handler);

        hashtable->size++;

        status = 0;
    }

    return status;
}

HandlerList_t* WmParamTable_GetHandlerList(WmParamTable_t* hashtable, int WmParamKey) {
    HandlerList_t* status = NULL;

    if (hashtable) {
        int idx = getHash(WmParamKey);
        int counter = 0;

        while (hashtable->entries[idx].WmParamKey != WmParamKey) {
            idx = (idx + 1) % MAX_WM_PARAM_AMOUNT;
            counter++;

            if (counter > (MAX_WM_PARAM_AMOUNT - 1)) {
                break;
            }

            if (hashtable->entries[idx].WmParamKey == WM_PARAM_TABLE_UNUSED_ID) {
                break;
            }
        }

        status = hashtable->entries[idx].handlerList;
    } else {
        DEBUG_ERROR("WmParamTable_GetHandlerList: handler table is null");
    }
    return status;
}

void WmParamTable_Free(WmParamTable_t* hashtable) {
    HandlerNode_t* curr;
    HandlerNode_t* next;
    WmParamEntry_t node;
    HandlerList_t* handlerList;

    if (hashtable) {
        for (int i = 0; i < MAX_WM_PARAM_AMOUNT; i++) {
            node = hashtable->entries[i];
            handlerList = node.handlerList;

            if (handlerList != NULL) {
                curr = node.handlerList->firstHandlerNode;

                while (curr != NULL) {
                    next = curr->nextHandlerNode;
                    free(curr);
                    curr = next;
                }
                free(handlerList);
                handlerList = NULL;
            }
        }

        free(hashtable);
        hashtable = NULL;
    } else {
        DEBUG_ERROR("WmParamTable_Free: handler table is null");
    }
}

bool WmParamHandlerTable_IdHasHandler(WmParamTable_t* hashtable, int msg) {
    bool status = false;
    HandlerList_t* handlerList = WmParamTable_GetHandlerList(hashtable, msg);

    if (handlerList) {
        MessageHandler_t handler = WmParamHashTable_GetFirstHandler(handlerList);

        if (handler) {
            status = true;
        }
    } else {
        DEBUG_ERROR("WmParamHandlerTable_IdHasHandle: handler list is null");
    }
    return status;
}

void WmParamHandlerTable_CallHandlersOfId(WmParamTable_t* hashtable, HWND hwnd, int id, WPARAM wparam, LPARAM lparam) {
    if (hashtable) {
        HandlerList_t* handlers = WmParamTable_GetHandlerList(hashtable, id);

        if (handlers) {
            HandlerNode_t* currentNode = handlers->firstHandlerNode;

            while (currentNode != NULL) {
                currentNode->handler(hwnd, id, wparam, lparam);
                currentNode = currentNode->nextHandlerNode;
            }
        } else {
            DEBUG_WARNING("WmParamHandlerTable_CallHandlersOfId: table has no handlers of id %d", id);
        }
    } else {
        DEBUG_ERROR("WmParamHandlerTable_CallHandlersOfId: handler table is null");
    }
}