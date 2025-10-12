#ifndef MENU_UI_H
#define MENU_UI_H

#include <stdbool.h>

#include "UiUtils.h"
#include "WmParamHashTable.h"
#include "container.h"
#include "coreWndProc.h"
#include "windows.h"


#define MENU_UI_CUSTOM_MESSAGE_ID 0x10000
#define MENU_UI_SUBMENU_LOAD_ID (MENU_UI_CUSTOM_MESSAGE_ID + 1)
#define MENU_UI_SUBMENU_DESTROY_ID (MENU_UI_CUSTOM_MESSAGE_ID + 2)

#define MENU_ID_ENCODE(groupId, submenuId) ((groupId) * 100 + (submenuId))
#define MENU_ID_GET_GROUP(menuId) ((menuId) / 100)
#define MENU_ID_GET_SUBMENU(menuId) ((menuId) % 100)

#define MENU_UI_MAX_NAME_LENGTH 30
#define MENU_UI_SUBMENU_MAX 99
#define MENU_UI_GROUP_MAX 10

typedef struct container_t container_t;
typedef struct ButtonWidget buttonWidget_t;
#define containerPos_t AbsolutePos_t

typedef struct {
    int SubmenuID;                     // unique submenu id
    WmParamTable_t* WmParamHashTable;  // hashtable of callbacks
    char name[MENU_UI_MAX_NAME_LENGTH];
    DynamicArray_t* containers;
} MenuUi_Submenu_t;

typedef struct submenuGroup {
    DynamicArray_t* submenus;
    int groupID;
    int activeSubmenuID;
} submenuGroup_t;

MenuUi_Submenu_t* MenuUi_SubmenuInit(const char name[MENU_UI_MAX_NAME_LENGTH], buttonWidget_t* button, submenuGroup_t* group);
void MenuUi_SubmenuCommandHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void MenuUi_SubmenuAddHandler(MessageHandler_t handler, int WmParamKey, MenuUi_Submenu_t* submenu);
void MenuUi_SubmenuAddLoadHandler(MessageHandler_t handler, MenuUi_Submenu_t* submenu);
void MenuUi_SubmenuAddDestroyHandler(MessageHandler_t handler, MenuUi_Submenu_t* submenu);
void MenuUi_CallAllActiveHandlers(HWND hwnd, int id, WPARAM wparam, LPARAM lparam);
void MenuUi_SubmenuAddContainer(MenuUi_Submenu_t* submenu, container_t* container);
submenuGroup_t* initSubmenuGroup(void);
void MenuUi_SetSubmenuActive(MenuUi_Submenu_t* menu);
MenuUi_Submenu_t* getActiveFromGroup(int groupID);

#endif