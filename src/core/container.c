#include "container.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"

#ifndef DISABLE_MENU
container_t * containerList[50];
int currentIdx = 0;
#endif

bool firstInit = false;

container_t * initContainer(containerPos_t pos, WmParamHandlerTable_t handlerTable){

   container_t * container = (container_t *) calloc(1, sizeof(container_t));

   #ifndef DISABLE_MENU

   #endif

}

LRESULT redrawContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   #ifndef DISABLE_MENU
   submenus[MENU_UI_SUBMENU_GET_IDX(MenuId)];


   #elif defined(DISABLE_MENU)
   for (int i = 0; i < currentIdx; i++){
      redrawContainer(containerList[i]);
   }
   #endif
   
}

void redrawContainer(container_t * container){
 
}

void resizeContainerContent(container_t * container){

}

void containerAddHWNDItem(HWND hwnd){

}

void containerAddCostumButton(char * name, MessageHandler_t callback){

}