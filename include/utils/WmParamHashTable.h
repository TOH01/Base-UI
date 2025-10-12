#ifndef WM_PARAM_HASH_TABLE_H
#define WM_PARAM_HASH_TABLE_H

#include <stdbool.h>
#include <windows.h>

#define MAX_WM_PARAM_AMOUNT 128
#define WM_PARAM_TABLE_UNUSED_ID -1

typedef LRESULT (*MessageHandler_t)(HWND, UINT, WPARAM, LPARAM);

typedef struct HandlerNote HandlerNode_t;

typedef struct HandlerNote {
    MessageHandler_t handler;
    bool active;
    HandlerNode_t* nextHandlerNode;
} HandlerNode_t;

typedef struct {
    HandlerNode_t* firstHandlerNode;
} HandlerList_t;

typedef struct {
    int WmParamKey;
    HandlerList_t* handlerList;
} WmParamEntry_t;

/**
 * @brief table, to efficiently store handlers
 *
 * WmParamTable_t
 * |__ entries[0]
 * |   |__ WmParamKey
 * |   |__ handlerList
 * |       |__firstHandlerNode -> HandlerNode_t -> nextHandlerNode -> ...
 * |__ entries[1]
 *     |__ WmParamKey
 *     |__ handlerList
 *         |__firstHandlerNode -> HandlerNode_t -> nextHandlerNode -> ...
 */
typedef struct {
    int size;
    WmParamEntry_t entries[MAX_WM_PARAM_AMOUNT];
    bool hasContainerHandlers;
} WmParamTable_t;

WmParamTable_t* WmParamTable_Init(void);
int WmParamTable_Insert(WmParamTable_t* hashtable, int WmParamKey, MessageHandler_t handler);
void WmParamTable_Free(WmParamTable_t* hashtable);
void WmParamHandlerTable_CallHandlersOfId(WmParamTable_t* hashtable, HWND hwnd, int id, WPARAM wparam, LPARAM lparam);
bool WmParamHandlerTable_IdHasHandler(WmParamTable_t* hashtable, int msg);

#endif