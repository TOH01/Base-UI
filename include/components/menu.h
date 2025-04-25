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

#define MENU_UI_MENU_WIDTH_PERCENTAGE UI_UTILS_PERCENT(20)
#define MENU_UI_BUTTON_SPACING_LEFT UI_UTILS_PERCENT(5)
#define MENU_UI_BUTTON_SPACING_RIGHT (MENU_UI_MENU_WIDTH_PERCENTAGE - 0.01f)
#define MENU_UI_BUTTON_SPACING_TOP UI_UTILS_PERCENT(2)
#define MENU_UI_BUTTON_HEIGHT UI_UTILS_PERCENT(4)

#define MENU_UI_BUTTON_WIDTH (MENU_UI_BUTTON_SPACING_RIGHT - MENU_UI_BUTTON_SPACING_LEFT)
#define MENU_UI_BUTTON_GET_START_Y(num) ((((num) + 1) * MENU_UI_BUTTON_SPACING_TOP) + ((num) * (MENU_UI_BUTTON_HEIGHT)))

#define MENU_UI_MAX_CONTAINER 5
#define MENU_UI_MAX_NAME_LENGTH 30

typedef struct container_t container_t;
#define containerPos_t CommonPos_t

extern container_t *topContainer;

typedef struct {
	int SubmenuID;                           // unique submenu id
	WmParamHandlerTable_t *WmParamHashTable; // hashtable of callbacks
	container_t *containers[MENU_UI_MAX_CONTAINER];
	int containerIdx;
	char name[MENU_UI_MAX_NAME_LENGTH];
} MenuUi_Submenu_t;

int MenuUi_SubmenuInit(char name[MENU_UI_MAX_NAME_LENGTH]);
void MenuUi_InitBaseHandlers(void);
void MenuUi_SubmenuCommandHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void MenuUi_SubmenuAddHandler(MessageHandler_t handler, int WmParamKey, int MenuId);
void MenuUi_SubmenuAddLoadHandler(MessageHandler_t handler, int id);
void MenuUi_SubmenuAddDestroyHandler(MessageHandler_t handler, int id);
MenuUi_Submenu_t *MenuUi_GetCurrentSubmenu(void);
container_t *MenuUi_SubmenuAddContainer(int MenuId, containerPos_t pos);

#endif