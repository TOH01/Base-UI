#ifndef MENU_UI_H
#define MENU_UI_H

#include <stdbool.h>
#include "UiUtils.h"

#define MENU_UI_ID_PERSISTANCE_MENU 100
#define MENU_UI_ID_STATE_MACHINE_MENU 200
#define MENU_UI_ID_ASSET_MENU 300

#define MENU_UI_STARTUP_MENU_ID MENU_UI_ID_PERSISTANCE_MENU

#define MENU_UI_MENU_WIDTH_PERCENTAGE 20
#define MENU_UI_BUTTON_SPACING_LEFT 5
#define MENU_UI_BUTTON_SPACING_RIGHT (MENU_UI_MENU_WIDTH_PERCENTAGE - 1)
#define MENU_UI_BUTTON_SPACING_TOP 2
#define MENU_UI_BUTTON_HEIGHT 4

#define MENU_UI_BUTTON_WIDTH (MENU_UI_BUTTON_SPACING_RIGHT - MENU_UI_BUTTON_SPACING_LEFT)
#define MENU_UI_BUTTON_GET_START_Y(num) ((((num) + 1) * MENU_UI_BUTTON_SPACING_TOP) + ((num) * (MENU_UI_BUTTON_HEIGHT)))

extern HPEN hPen;
extern PAINTSTRUCT ps;
extern HDC hdc;

typedef struct {

    void (*SubmenuRenderContent)(HWND);             // callback to render content which is initialized as handle
    void (*SubmenuDestroyContent)(void);            // callback to deinitialize content which is initialized as handle
    void (*SubmenuResizeContent)(int, int);         // callback to resize content which is initialized as handle
    void (*SubmenuDrawContent)(HWND, int, int);     // callback to (re-) draw content, BeginPaint() and EndPaint() will be called automatically, do not call them in here
    void (*SubmenuWMParamCallback)(HWND, WPARAM);   // will be called when a WM Command, for example a button click, is triggered on the specific submenu

} MenuUi_Submenu_Callback_t;

typedef struct {
    
    int SubmenuID;                                          // unique submenu id
    button_t SubmenuLoadButton;                             // button struct for button in the main menu to load submenu, will be initialized by MenuUi.c
    HWND hSubmenuLoadButton;                                // handle of described button
    MenuUi_Submenu_Callback_t SubmenuCallbacks;             // various callbacks, see MenuUi_Submenu_Callback_t for details

} MenuUi_Submenu_t;


bool MenuUi_IsSidebarCommand(WORD commandId);

void MenuUi_RenderMenuButtons(HWND hwnd);
void MenuUi_ResizeMenuUi(int width, int height);
void MenuUi_DrawMenuUi(HWND hwnd, int width, int height);

void MenuUi_DrawMenuContent(HWND hwnd, int currentWidth, int currentHeight);
void MenuUi_ResizeMenuContent(int width, int height);
void MenuUi_SubmenuSwap(HWND hwnd, int menuId);

void MenuUi_SubmenuInit(char name[30], MenuUi_Submenu_Callback_t callbacks);
void MenuUi_SubmenuWmParamCallback(HWND hwnd, WPARAM wParam);
void MenuUi_SubmenuInitAll(void);

#endif