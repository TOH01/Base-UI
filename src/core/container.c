#include "container.h"
#include "WmParamHashTable.h"
#include "MenuUi.h"
#include <stdio.h>

container_t * containerList[50];
int currentWindowContainerIdx = 0;

container_t * topContainer = NULL;
container_t ** topContainerList;
int topContainerIdx;

movingContainer_t movingContainer;

BaseWidget_t * hoverCandidate = NULL;
BaseWidget_t * lastHoverCandidate = NULL;
DWORD hoverStartTime = 0;
#define HOVER_TIMER_ID 1
#define HOVER_DELAY_MS 300  // 300ms delay

void redrawContainer(container_t * container){
   SelectObject(currentWindowState.memHDC, currentWindowState.hPen);
   UiUitls_DrawRectangleRelative(container->pos);
}

void redrawContainerList(container_t * containers[], int num){
   for (int i = 0; i < num; i++){
      redrawContainer(containers[i]);
      if(!(movingContainer.action && containers[i] == movingContainer.container)){
         renderWidgetList(containers[i]->widgetList);
      }
      
   }
}

void moveContainerOnTop(container_t * containerArray[], int idx){
   

   if (topContainer != containerArray[idx]){
      
      if(topContainer){
         topContainerList[topContainerIdx] = topContainer;
      }
      
      topContainer = containerArray[idx];
      topContainerList = containerArray;
      topContainerIdx = idx;

   }

}


/* 
return: true if click was inside of or on border of container from param containers[]
*/
bool containerListLButtonDown(container_t * containers[], int num, int x, int y){
   
   // reset selected input, has to be done before widget callback
   activeInput = NULL;
   activeTextDump = NULL;
   
   for (int i = 0; i < num; i++){
      
      if (UiUtils_CoordinateIsOnBorderOf(x, y, containers[i]->borderWitdh, containers[i]->pos)){
         if (!movingContainer.action){
            movingContainer.startPos = containers[i]->pos;
            movingContainer.action = UiUtils_CoordinateIsOnBorderOf(x, y, containers[i]->borderWitdh, containers[i]->pos);
            movingContainer.container = containers[i];
            movingContainer.mouseStartX = x;
            movingContainer.mouseStartY = y;
            return 1;
         }
      }
      
      if (UiUtils_CoordinateIsInsideOf(x, y, containers[i]->pos)){
         
         BaseWidget_t * widget = widgetClicked(x, y, containers[i]->widgetList);

         if (widget){
            widget->onClick(widget, x, y);
            InvalidateRect(currentWindowState.hwnd, NULL, FALSE); // redraw for interactive widgets like checkboxes, which need redraw on click
            return 1;
         }

         if (!movingContainer.action){
            movingContainer.startPos = containers[i]->pos;
            movingContainer.action = CONTAINER_MOVE_ACTION;
            movingContainer.container = containers[i];
            movingContainer.mouseStartX = x;
            movingContainer.mouseStartY = y;
            moveContainerOnTop(containers, i);
            return 1;
         }

      }
   
   }

   return 0;

}


LRESULT redrawContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   
   #ifndef DISABLE_MENU
  
   MenuUi_Submenu_t * submenu = getGurrentSubmenu();

   redrawContainerList(submenu->containers, submenu->containerIdx);

   #endif

   redrawContainerList(containerList, currentWindowContainerIdx);

   if(topContainer){
      redrawContainerList(&topContainer, 1);
   }
   
}

LRESULT resizeContainers(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   InvalidateRect(hwnd, NULL, FALSE);
}

LRESULT LButtonDownCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   int x = LOWORD(lParam); // Horizontal position in client area
   int y = HIWORD(lParam); // Vertical position in client area

   bool widgetClicked = 0;

   if (topContainer){
      widgetClicked = widgetClicked | containerListLButtonDown(&topContainer, 1, x, y);
   }
   
   if(!widgetClicked){
      widgetClicked = widgetClicked | containerListLButtonDown(containerList, currentWindowContainerIdx, x, y);
   }

   #ifndef DISABLE_MENU
   if(!widgetClicked){
      MenuUi_Submenu_t * submenu = getGurrentSubmenu();

      widgetClicked = widgetClicked | containerListLButtonDown(submenu->containers, submenu->containerIdx, x, y);
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

   else {
      
      hoverCandidate = NULL;
      bool overlapFlag = false;
      
      if(!hoverCandidate){
         
         if(topContainer){
            hoverCandidate = widgetClicked(x, y, topContainer->widgetList);
            overlapFlag = UiUtils_CoordinateIsInsideOf(x, y, topContainer->pos);
         }
         
      }
      
      if(!hoverCandidate){
         for (int i = 0; i < currentWindowContainerIdx; i++){
            if(!hoverCandidate && !overlapFlag){
               hoverCandidate = widgetClicked(x, y, containerList[i]->widgetList);
               overlapFlag = UiUtils_CoordinateIsInsideOf(x, y, containerList[i]->pos);
            }
         }
      }

      if(!hoverCandidate){
         for (int i = 0; i < getGurrentSubmenu()->containerIdx; i++){
            if(!hoverCandidate && !overlapFlag){
               hoverCandidate = widgetClicked(x, y, getGurrentSubmenu()->containers[i]->widgetList);
               overlapFlag = UiUtils_CoordinateIsInsideOf(x, y, getGurrentSubmenu()->containers[i]->pos);
            }
         }
      }

      
      if (hoverCandidate != lastHoverCandidate) {
         if (hoverCandidate) {
             // Hovered element changed, restart timer
             hoverStartTime = GetTickCount();
             SetTimer(currentWindowState.hwnd, HOVER_TIMER_ID, HOVER_DELAY_MS, NULL);
         } else {
             // Mouse moved away, stop timer
             KillTimer(currentWindowState.hwnd, HOVER_TIMER_ID);
         }
         lastHoverCandidate = hoverCandidate;
     }

   }   
}

LRESULT CALLBACK TimerCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
   if (msg == WM_TIMER && wParam == HOVER_TIMER_ID) {
      if (lastHoverCandidate) {
          if (GetTickCount() - hoverStartTime >= HOVER_DELAY_MS) {
              // Hover effect triggered
              printf("HOVER TRIGGERED\n");
          }
      } else {
          // No hover candidate, stop timer
          KillTimer(hwnd, HOVER_TIMER_ID);
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
      WmParamHanderTable_Insert(handlerTable, WM_TIMER, &TimerCallback);

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
