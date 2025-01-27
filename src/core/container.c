#include "container.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"
#include <stdio.h>

#ifndef DISABLE_MENU
container_t * containerList[50];
int currentIdx = 0;
#endif

movingContainer_t movingContainer;

void redrawContainer(container_t * container){
   SelectObject(currentWindowState.memHDC, currentWindowState.hPen);
   UiUitls_DrawRectangleRelative(container->pos);
}

LRESULT redrawContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   
   #ifndef DISABLE_MENU
  
   MenuUi_Submenu_t * submenu = getGurrentSubmenu();

   for (int i = 0; i < submenu->containerIdx; i++){
      redrawContainer(submenu->containers[i]);
      if(!movingContainer.action){
         renderWidgetList(submenu->containers[i]->widgetList);
      }
      
   }

   #elif defined(DISABLE_MENU)
   for (int i = 0; i < currentIdx; i++){
      redrawContainer(containerList[i]);
      renderWidgetList(submenu->containers[i]->widgetList);
   }
   #endif

}

LRESULT resizeContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   InvalidateRect(hwnd, NULL, FALSE);
}

void moveContainerOnTop(container_t * containerArray[], int idx){
   
   if (idx == 0){
      return; //container is already at the top
   }
   
   container_t * topContainer = containerArray[idx];

   for (int i = idx; i > 0; i--){
      containerArray[i] = containerArray[i -1];
   }

   containerArray[0] = topContainer;

}

LRESULT LButtonDownCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   int x = LOWORD(lParam); // Horizontal position in client area
   int y = HIWORD(lParam); // Vertical position in client area

   #ifndef DISABLE_MENU
  
   MenuUi_Submenu_t * submenu = getGurrentSubmenu();

   for (int i = 0; i < submenu->containerIdx; i++){
      
      if (UiUtils_CoordinateIsOnBorderOf(x, y, submenu->containers[i]->borderWitdh, submenu->containers[i]->pos)){
         if (!movingContainer.action){
            movingContainer.startPos = submenu->containers[i]->pos;
            movingContainer.action = UiUtils_CoordinateIsOnBorderOf(x, y, submenu->containers[i]->borderWitdh, submenu->containers[i]->pos);
            movingContainer.container = submenu->containers[i];
            movingContainer.mouseStartX = x;
            movingContainer.mouseStartY = y;
            return 1;
         }
      }
      
      if (UiUtils_CoordinateIsInsideOf(x, y, submenu->containers[i]->pos)){
         
         BaseWidget_t * widget = widgetClicked(x, y, submenu->containers[i]->widgetList);

         if (widget){
            widget->onClick(widget, x, y);
            InvalidateRect(hwnd, NULL, FALSE); // redraw for interactive widgets like checkboxes, which need redraw on click
            return 1;
         }

         if (!movingContainer.action){
            movingContainer.startPos = submenu->containers[i]->pos;
            movingContainer.action = CONTAINER_MOVE_ACTION;
            movingContainer.container = submenu->containers[i];
            movingContainer.mouseStartX = x;
            movingContainer.mouseStartY = y;
            return 1;
         }
      }
   
   }

   #elif defined(DISABLE_MENU)
   for (int i = 0; i < currentIdx; i++){
      printf("%d", UiUtils_CoordinateIsInsideOf(x, y, containerList[i]->pos));
      printf("%d\n", UiUtils_CoordinateIsOnBorderOf(x, y, containerList[i]->borderWidth, containerList[i]->pos));
   }
   #endif 

   InvalidateRect(hwnd, NULL, FALSE);

}

LRESULT LButtonUpCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   if (movingContainer.action){
      movingContainer.action = 0;

      #ifndef DISABLE_MENU
  
      MenuUi_Submenu_t * submenu = getGurrentSubmenu();

      for (int i = 0; i < submenu->containerIdx; i++){

            WidgetList_t * list = submenu->containers[i]->widgetList;

            updatePosToContainerList(movingContainer.container->pos, list);
      }

      #elif defined(DISABLE_MENU)

      #endif

      InvalidateRect(hwnd, NULL, FALSE);

   }

}

LRESULT MouseMoveCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   int x = LOWORD(lParam); // Horizontal position in client area
   int y = HIWORD(lParam); // Vertical position in client area

   if (movingContainer.action){
      
      float x_relative = (float) x / (float) currentWindowState.currentWidth;
      float y_relative = (float) y / (float) currentWindowState.currentHeight;

      float start_x_relative = (float) movingContainer.mouseStartX / (float) currentWindowState.currentWidth;
      float start_y_relative = (float) movingContainer.mouseStartY / (float) currentWindowState.currentHeight;

      if(movingContainer.action == CONTAINER_MOVE_ACTION){
         movingContainer.container->pos.spacingLeft = movingContainer.startPos.spacingLeft + (x_relative - start_x_relative);
         movingContainer.container->pos.spacingTop = movingContainer.startPos.spacingTop + (y_relative - start_y_relative);
         movingContainer.container->pos.width = movingContainer.startPos.width + (x_relative - start_x_relative);
         movingContainer.container->pos.height = movingContainer.startPos.height + (y_relative - start_y_relative);
      }

      else {
         switch (movingContainer.action)
         {
         case TOP:
            movingContainer.container->pos.spacingTop = movingContainer.startPos.spacingTop + (y_relative - start_y_relative);
            break;
         case BOTTOM:
            movingContainer.container->pos.height = movingContainer.startPos.height + (y_relative - start_y_relative);
            break;
         case LEFT:
            movingContainer.container->pos.spacingLeft = movingContainer.startPos.spacingLeft + (x_relative - start_x_relative);
            break;
         case RIGHT:
            movingContainer.container->pos.width = movingContainer.startPos.width + (x_relative - start_x_relative);
            break;
         default:
            break;
         }
      }

      InvalidateRect(hwnd, NULL, FALSE);

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

   container->widgetList = initWidgetList();

   return container;

}


void containerAddWidget(container_t * container, BaseWidget_t * widget){
   widget->pos = getPosToContainer(container->pos, widget->pos);
   addWidget(container->widgetList, widget);
}