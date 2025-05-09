#ifndef MENU_UI_H
#define MENU_UI_H

#include "UiUtils.h"
#include "WmParamHashTable.h"
#include "container.h"
#include "coreWndProc.h"
#include "windows.h"
#include <stdbool.h>

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
#define containerPos_t CommonPos_t

typedef struct {
	int SubmenuID;                           // unique submenu id
	WmParamHandlerTable_t *WmParamHashTable; // hashtable of callbacks
	char name[MENU_UI_MAX_NAME_LENGTH];
	DynamicArray_t *containers;
} MenuUi_Submenu_t;

typedef struct submenuGroup {
	DynamicArray_t * submenus;
	int groupID;
	int activeSubmenuID;
} submenuGroup_t;

MenuUi_Submenu_t * MenuUi_SubmenuInit(char name[MENU_UI_MAX_NAME_LENGTH], buttonWidget_t *button, submenuGroup_t *group);
void MenuUi_InitBaseHandlers(void);
void MenuUi_SubmenuCommandHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void MenuUi_SubmenuAddHandler(MessageHandler_t handler, int WmParamKey, MenuUi_Submenu_t * submenu);
void MenuUi_SubmenuAddLoadHandler(MessageHandler_t handler, MenuUi_Submenu_t * submenu);
void MenuUi_SubmenuAddDestroyHandler(MessageHandler_t handler, MenuUi_Submenu_t * submenu);
MenuUi_Submenu_t *MenuUi_CallAllActiveHandlers(HWND hwnd, int id, WPARAM wparam, LPARAM lparam);
container_t *MenuUi_SubmenuAddContainer(MenuUi_Submenu_t * submenu, containerPos_t pos);
submenuGroup_t *initSubmenuGroup();

#endif