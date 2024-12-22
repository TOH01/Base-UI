#ifndef WM_PARAM_HASH_TABLE_H
#define WM_PARAM_HASH_TABLE_H

#include <windows.h>

#define MAX_WM_PARAM_AMOUNT 128

typedef LRESULT (*MessageHandler_t)(HWND, UINT, WPARAM, LPARAM);

typedef struct HashNode{
    int key;
    MessageHandler_t handler;
} HashNode_t;

typedef struct {
    int size;
    HashNode_t content[MAX_WM_PARAM_AMOUNT];
} HashTable_t;

HashTable_t * WmParamHashTable_Init();
int WmParamHashTable_Insert(HashTable_t * hashtable, int key, MessageHandler_t handler);
MessageHandler_t WmParamHashTable_Get(HashTable_t * hashtable, int key);
void WmParamHashTable_Destroy(HashTable_t * hashtable);

#endif