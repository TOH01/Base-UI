#include "container.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"
#include <stdio.h>

#ifndef DISABLE_MENU
container_t * containerList[50];
int currentIdx = 0;
#endif

bool lockMovement = 0;
bool resizing = 0;
bool moving = 0;
int start_x = 0;
int start_y = 0;

containerPos_t containerStartPos;

container_t * movingContainer = NULL;

void redrawContainer(container_t * container){
   SelectObject(currentWindowState.hdc, currentWindowState.hPen);
   UiUitls_DrawRectangleRelative(container->pos);
}

LRESULT redrawContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   
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

}

LRESULT resizeContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   
}

LRESULT LButtonDownCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   int x = LOWORD(lParam); // Horizontal position in client area
   int y = HIWORD(lParam); // Vertical position in client area

   #ifndef DISABLE_MENU
  
   MenuUi_Submenu_t * submenu = getGurrentSubmenu();

   for (int i = 0; i < submenu->containerIdx; i++){
      if (!(lockMovement) && (UiUtils_CoordinateIsOnBorderOf(x, y, submenu->containers[i]->borderWitdh, submenu->containers[i]->pos))){
         lockMovement = 1;
         resizing = 1;
         start_x = x;
         start_y = y;
         movingContainer = submenu->containers[i];
         containerStartPos = submenu->containers[i]->pos;
      }
      if (!(lockMovement) && (UiUtils_CoordinateIsInsideOf(x, y, submenu->containers[i]->pos))){
         lockMovement = 1;
         moving = 1;
         start_x = x;
         start_y = y;
         movingContainer = submenu->containers[i];
         containerStartPos = submenu->containers[i]->pos;
      }
   }

   #elif defined(DISABLE_MENU)
   for (int i = 0; i < currentIdx; i++){
      printf("%d", UiUtils_CoordinateIsInsideOf(x, y, containerList[i]->pos));
      printf("%d\n", UiUtils_CoordinateIsOnBorderOf(x, y, containerList[i]->borderWidth, containerList[i]->pos));
   }
   #endif 
}

LRESULT LButtonUpCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   if (lockMovement){
      lockMovement = 0;
      moving = 0;
      resizing = 0;
      start_x = 0;
      start_y = 0;
      movingContainer = NULL;
      InvalidateRect(hwnd, NULL, TRUE);
   }
}

LRESULT MouseMoveCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   int x = LOWORD(lParam); // Horizontal position in client area
   int y = HIWORD(lParam); // Vertical position in client area

   if (lockMovement){
      float x_relative = (float) x / (float) currentWindowState.currentWidth;
      float y_relative = (float) y / (float) currentWindowState.currentHeight;

      float start_x_relative = (float) start_x / (float) currentWindowState.currentWidth;
      float start_y_relative = (float) start_y / (float) currentWindowState.currentHeight;

      if(moving){
         movingContainer->pos.spacingLeft = containerStartPos.spacingLeft + (x_relative - start_x_relative);
         movingContainer->pos.spacingTop = containerStartPos.spacingTop + (y_relative - start_y_relative);
         movingContainer->pos.width = containerStartPos.width + (x_relative - start_x_relative);
         movingContainer->pos.height = containerStartPos.height + (y_relative - start_y_relative);
      }

      if(resizing){

      }

   }

}


container_t * initContainer(containerPos_t pos, WmParamHandlerTable_t * handlerTable){

   container_t * container = (container_t *) calloc(1, sizeof(container_t));

   container->pos = pos;

   if (!handlerTable->hasContainerHandlers){
      WmParamHanderTable_Insert(handlerTable, WM_PAINT, &redrawContainers);
      WmParamHanderTable_Insert(handlerTable, WM_SIZE, &resizeContainers);
      WmParamHanderTable_Insert(handlerTable, WM_LBUTTONDOWN, &LButtonDownCallback);
      WmParamHanderTable_Insert(handlerTable, WM_LBUTTONUP, &LButtonUpCallback);
      WmParamHanderTable_Insert(handlerTable, WM_MOUSEMOVE, &MouseMoveCallback);

      handlerTable->hasContainerHandlers = true;
   }

   container->borderWitdh = 2; //TODO: make container drawable with different borderWidths

   return container;

}


void containerAddHWNDItem(HWND hwnd){

}

void containerAddCostumButton(char * name, MessageHandler_t callback){

}