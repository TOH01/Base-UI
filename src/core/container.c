#include "container.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"
#include <stdio.h>

container_t * containerList[50];
int currentWindowContainerIdx = 0;

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
      if(!(movingContainer.action && submenu->containers[i] == movingContainer.container)){
         renderWidgetList(submenu->containers[i]->widgetList);
      }
      
   }

   #endif

   for (int i = 0; i < currentWindowContainerIdx; i++){
      redrawContainer(containerList[i]);
      
      if(! (movingContainer.action && containerList[i] == movingContainer.container)){
         renderWidgetList(containerList[i]->widgetList);
      }
   }
   
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

      for (int i = 0; i < currentWindowContainerIdx; i++){
      
      if (UiUtils_CoordinateIsOnBorderOf(x, y, containerList[i]->borderWitdh, containerList[i]->pos)){
         if (!movingContainer.action){
            movingContainer.startPos = containerList[i]->pos;
            movingContainer.action = UiUtils_CoordinateIsOnBorderOf(x, y, containerList[i]->borderWitdh, containerList[i]->pos);
            movingContainer.container = containerList[i];
            movingContainer.mouseStartX = x;
            movingContainer.mouseStartY = y;
            return 1;
         }
      }
      
      if (UiUtils_CoordinateIsInsideOf(x, y, containerList[i]->pos)){
         
         BaseWidget_t * widget = widgetClicked(x, y, containerList[i]->widgetList);

         if (widget){
            widget->onClick(widget, x, y);
            InvalidateRect(hwnd, NULL, FALSE); // redraw for interactive widgets like checkboxes, which need redraw on click
            return 1;
         }

         if (!movingContainer.action){
            movingContainer.startPos = containerList[i]->pos;
            movingContainer.action = CONTAINER_MOVE_ACTION;
            movingContainer.container = containerList[i];
            movingContainer.mouseStartX = x;
            movingContainer.mouseStartY = y;
            moveContainerOnTop(containerList, i);
            return 1;
         }

      }
   
   }

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
            moveContainerOnTop(submenu->containers, i);
            return 1;
         }
      }
   
   }

   #endif

}

LRESULT LButtonUpCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   if (movingContainer.action){
      
      updatePosToContainerList(movingContainer.container->pos, movingContainer.container->widgetList);

      movingContainer.action = 0;

   }

   InvalidateRect(hwnd, NULL, FALSE);

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

container_t * windowAddContainer(CommonPos_t pos){
   
   #ifndef DISABLE_MENU
   // each submenu has their own container handlers, which will automatically handle shared containers aswell
   // setting hasContainerHandles to true, will prevent handlers being called twice

   currentWindowState.wmParamHashTable->hasContainerHandlers = 1; 
   #endif
   
   
   container_t * container = initContainer(pos, currentWindowState.wmParamHashTable);

   containerList[currentWindowContainerIdx] = container;

   currentWindowContainerIdx++;

   return container;
}