#include "costumSlider.h"
#include "common.h"
#include "widget.h"
#include "UiUtils.h"
#include <stdio.h>

sliderWidget_t *activeSlider = NULL;
bool sliderHandlersRegistered = false;

void drawSlider(BaseWidget_t * baseWidget){
    SelectObject(currentWindowState.memHDC, currentWindowState.hPen);

    sliderWidget_t * slider = (sliderWidget_t *) baseWidget;
    CommonPos_t pointPos = baseWidget->pos;
    CommonPos_t barPos = baseWidget->pos;

    float barWidth = baseWidget->pos.width - baseWidget->pos.spacingLeft;
    float barHeight = baseWidget->pos.height - baseWidget->pos.spacingTop;

    barPos.spacingTop = baseWidget->pos.spacingTop + (barHeight  * 0.4);
    barPos.height = baseWidget->pos.spacingTop + (barHeight * 0.6);

    float pointWidth = barWidth / (float) (slider->range);

    float pointX = baseWidget->pos.spacingLeft + ((*slider->value) * (barWidth - pointWidth)) / (slider->range - 1);
    
    pointPos.spacingLeft = pointX;
    pointPos.width = pointX + pointWidth;
    
    pointPos.spacingTop = baseWidget->pos.spacingTop + (barHeight  * 0.2);
    pointPos.height = baseWidget->pos.spacingTop + (barHeight * 0.8);

    UiUtils_DrawRoundRectangleRelative(barPos);
    UiUtils_DrawEllipseRelative(pointPos);

}

void onClickSlider(BaseWidget_t * baseWidget, int x, int y){
    sliderWidget_t * slider = (sliderWidget_t *) baseWidget;

    float width = (slider->baseWidget.pos.width - slider->baseWidget.pos.spacingLeft);

    for (int i = 0; i < slider->range; i++){
        CommonPos_t pos = baseWidget->pos;
        pos.spacingLeft = (baseWidget->pos.spacingLeft + ((width / slider->range) * i));
        pos.width = baseWidget->pos.spacingLeft + ((width / slider->range) * (i + 1));

        if (UiUtils_CoordinateIsInsideOf(x, y, pos)){
            *(slider->value) = i;
        }

    }

    activeSlider = slider;

}

LRESULT mouseMoveSlider(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    if(activeSlider){
        
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hwnd, &mousePos);
        
        float width = (activeSlider->baseWidget.pos.width - activeSlider->baseWidget.pos.spacingLeft);

        for (int i = 0; i < activeSlider->range; i++){
            CommonPos_t pos = activeSlider->baseWidget.pos;
            pos.spacingLeft = (activeSlider->baseWidget.pos.spacingLeft + ((width / activeSlider->range) * i));
            pos.width = activeSlider->baseWidget.pos.spacingLeft + ((width / activeSlider->range) * (i + 1));

            if (UiUtils_CoordinateIsInsideOf(mousePos.x, mousePos.y, pos)){
                *(activeSlider->value) = i;
            }

        }

        InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
    }
}

LRESULT mouseReleaseSlider(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    if(activeSlider){
        activeSlider = NULL;
    }
}

sliderWidget_t * initSlider(CommonPos_t pos, int * value, int range){
    sliderWidget_t * slider = (sliderWidget_t *) calloc(1, sizeof(sliderWidget_t));
    slider->baseWidget.pos = pos;
    slider->baseWidget.initPos = pos;
    slider->baseWidget.drawHandler = &drawSlider;
    slider->baseWidget.onClick = &onClickSlider;

    slider->range = range;
    slider->value = value;

    if(!sliderHandlersRegistered){
        WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_MOUSEMOVE, (void *)&mouseMoveSlider);
        WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_LBUTTONUP, (void *)&mouseReleaseSlider);
        sliderHandlersRegistered = true;
    }

    return slider;
}