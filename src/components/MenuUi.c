#include "UiUtils.h"
#include "common.h"
#include "container.h"
#include "customButton.h"
#include "menu.h"
#include "titlbar.h"
#include <stdio.h>
#include <windows.h>

DynamicArray_t *submenuGroups = NULL;

MenuUi_Submenu_t *getActiveFromGroup(int groupID) {
	submenuGroup_t *group = DynamicArray_get(submenuGroups, groupID);
	MenuUi_Submenu_t *submenu = DynamicArray_get(group->submenus, group->activeSubmenuID);
	return submenu;
}

submenuGroup_t *initSubmenuGroup(void) {
	submenuGroup_t *group = (submenuGroup_t *)calloc(1, sizeof(submenuGroup_t));

	if (submenuGroups == NULL) {
		submenuGroups = DynamicArray_init(10);
	}

	if (submenuGroups->size >= MENU_UI_GROUP_MAX) {
		return NULL;
	}

	group->submenus = DynamicArray_init(10);
	group->activeSubmenuID = 0;
	group->groupID = submenuGroups->size;

	DynamicArray_Add(submenuGroups, group);

	return group;
}

container_t *MenuUi_SubmenuAddContainer(MenuUi_Submenu_t *submenu, containerPos_t pos) {

	container_t *container = windowAddContainer(pos);

	// we want first initialized menu to be visibile
	if (MENU_ID_GET_SUBMENU(submenu->SubmenuID) == 0) {
		container->visible = 1;
	} else {
		container->visible = 0;
	}

	if (submenu->containers == NULL) {
		submenu->containers = DynamicArray_init(10);
	}

	DynamicArray_Add(submenu->containers, container);

	return container;
}

void MenuUi_SubmenuSwap(int menuId) {

	// when swapping submenu reset active input
	activeInput = NULL;
	activeTextDump = NULL;

	int groupID = MENU_ID_GET_GROUP(menuId);
	int submenuID = MENU_ID_GET_SUBMENU(menuId);

	submenuGroup_t *group = (submenuGroup_t *)DynamicArray_get(submenuGroups, groupID);

	// if already selected return, do not do redundant calculations
	if (group->activeSubmenuID == submenuID) {
		return;
	}

	// make the currently visible submenu invsible
	MenuUi_Submenu_t *submenu = (MenuUi_Submenu_t *)DynamicArray_get(group->submenus, group->activeSubmenuID);
	container_t *container = NULL;

	for (int i = 0; i < submenu->containers->size; i++) {
		container = DynamicArray_get(submenu->containers, i);

		if (container) {
			container->visible = 0;
		}
	}

	MenuUi_SubmenuCommandHandler(currentWindowState.hwnd, MENU_UI_SUBMENU_DESTROY_ID, 0, 0);

	submenu = (MenuUi_Submenu_t *)DynamicArray_get(group->submenus, submenuID);

	for (int i = 0; i < submenu->containers->size; i++) {
		container = DynamicArray_get(submenu->containers, i);

		if (container) {
			container->visible = 1;
		}
	}

	group->activeSubmenuID = submenuID;

	MenuUi_SubmenuCommandHandler(currentWindowState.hwnd, MENU_UI_SUBMENU_LOAD_ID, 0, 0);

	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

void MenuUi_CallAllActiveHandlers(HWND hwnd, int msg, WPARAM wparam, LPARAM lparam) {

	if (submenuGroups != NULL) {
		for (int i = 0; i < submenuGroups->size; i++) {
			WmParamHandlerTable_CallHandlersOfId(getActiveFromGroup(i)->WmParamHashTable, hwnd, msg, wparam, lparam);
		}
	}
}

MenuUi_Submenu_t * MenuUi_SubmenuInit(const char name[MENU_UI_MAX_NAME_LENGTH], buttonWidget_t *button, submenuGroup_t *group) {

	if (group->submenus->size >= MENU_UI_SUBMENU_MAX) {
		return NULL;
	}

	MenuUi_Submenu_t *submenu = (MenuUi_Submenu_t *)calloc(1, sizeof(MenuUi_Submenu_t));

	submenu->SubmenuID = MENU_ID_ENCODE(group->groupID, group->submenus->size);

	submenu->WmParamHashTable = WmParamHandlerTable_Init();

	DynamicArray_Add(group->submenus, submenu);

	memcpy(submenu->name, name, MENU_UI_MAX_NAME_LENGTH);

	button->id = submenu->SubmenuID;
	button->onClickUserCallback = &MenuUi_SubmenuSwap;

	customButton_setButtonText(button, name);

	return submenu;
}

void MenuUi_SubmenuCommandHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if(submenuGroups == NULL){
		return;
	}
	
	for (int i = 0; i < submenuGroups->size; i++) {
		if (getActiveFromGroup(i)->WmParamHashTable != NULL) {

			if (WmParamHandlerTable_IdHasHandler(getActiveFromGroup(i)->WmParamHashTable, msg)) {
				WmParamHandlerTable_CallHandlersOfId(getActiveFromGroup(i)->WmParamHashTable, hwnd, msg, wParam, lParam);
			}
		}
	}
}

void MenuUi_SubmenuAddHandler(MessageHandler_t handler, int WmParamKey, MenuUi_Submenu_t *submenu) { WmParamHanderTable_Insert(submenu->WmParamHashTable, WmParamKey, handler); }

void MenuUi_SubmenuAddLoadHandler(MessageHandler_t handler, MenuUi_Submenu_t *submenu) { WmParamHanderTable_Insert(submenu->WmParamHashTable, MENU_UI_SUBMENU_LOAD_ID, handler); }

void MenuUi_SubmenuAddDestroyHandler(MessageHandler_t handler, MenuUi_Submenu_t *submenu) { WmParamHanderTable_Insert(submenu->WmParamHashTable, MENU_UI_SUBMENU_DESTROY_ID, handler); }
