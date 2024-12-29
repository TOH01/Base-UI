#ifndef WM_PARAM_HASH_TABLE_H
#define WM_PARAM_HASH_TABLE_H

#include <windows.h>
#include <stdbool.h>

#define MAX_WM_PARAM_AMOUNT 128
#define WM_PARAM_TABLE_UNUSED_ID -1

typedef LRESULT (*MessageHandler_t)(HWND, UINT, WPARAM, LPARAM);

typedef struct ListNode HandlerNode_t;

typedef struct ListNode{
    MessageHandler_t handler;
    HandlerNode_t * nextHandlerNode;
} HandlerNode_t;

typedef struct List{
    HandlerNode_t * firstHandlerNode;
} HandlerList_t;

typedef struct HashNode{
    int WmParamKey;
    HandlerList_t * WmParamHandler;
} WmParamHandlerNode_t;

typedef struct {
    int size;
    WmParamHandlerNode_t content[MAX_WM_PARAM_AMOUNT];
} WmParamHandlerTable_t;

WmParamHandlerTable_t * WmParamHandlerTable_Init();
int WmParamHanderTable_Insert(WmParamHandlerTable_t * hashtable, int WmParamKey, MessageHandler_t handler);
void WmParamHandlerTable_Destroy(WmParamHandlerTable_t * hashtable);
void WmParamHandlerTable_CallHandlersOfId(WmParamHandlerTable_t * hashtable, HWND hwnd, int id, WPARAM wparam, LPARAM lparam);
bool WmParamHandlerTable_IdHasHandler(WmParamHandlerTable_t * hashtable, int msg);

#endif