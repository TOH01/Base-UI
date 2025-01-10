#include "container.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"
#include <stdio.h>

#ifndef DISABLE_MENU
container_t * containerList[50];
int currentIdx = 0;
#endif

void redrawContainer(container_t * container){
   SelectObject(currentWindowState.hdc, currentWindowState.hPen);
   Rectangle(currentWindowState.hdc, container->pos.spacingLeft, container->pos.spacingTop, container->pos.width, container->pos.height);
}

LRESULT redrawContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   
   printf("TEST\n");

   currentWindowState.hdc = BeginPaint(hwnd, &currentWindowState.ps);
    
   #ifndef DISABLE_MENU
  
   MenuUi_Submenu_t * submenu = getGurrentSubmenu();

   for (int i = 0; i < submenu->containerIdx; i++){
      redrawContainer(submenu->containers[i]);
   }

   #elif defined(DISABLE_MENU)
   for (int i = 0; i < currentIdx; i++){
      redrawContainer(containerList[i]);
   }
   #endif

   EndPaint(hwnd, &currentWindowState.ps);
   
}

container_t * initContainer(containerPos_t pos, WmParamHandlerTable_t * handlerTable){

   container_t * container = (container_t *) calloc(1, sizeof(container_t));

   container->pos = pos;

   if (handlerTable->hasContainerHandlers){
      WmParamHanderTable_Insert(handlerTable, WM_PAINT, &redrawContainers);

      handlerTable->hasContainerHandlers = true;
   }

}

void resizeContainerContent(container_t * container){

}

void containerAddHWNDItem(HWND hwnd){

}

void containerAddCostumButton(char * name, MessageHandler_t callback){

}