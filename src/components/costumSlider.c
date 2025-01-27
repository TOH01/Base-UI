#include "costumSlider.h"
#include "common.h"
#include "widget.h"
#include "UiUtils.h"
#include <stdio.h>

void drawSlider(BaseWidget_t * baseWidget){
    SelectObject(currentWindowState.memHDC, currentWindowState.hPen);

    sliderWidget_t * slider = (sliderWidget_t *) baseWidget;

    float width = (slider->baseWidget.pos.width - slider->baseWidget.pos.spacingLeft);

    for (int i = 0; i < slider->range; i++){
        CommonPos_t pos = baseWidget->pos;
        pos.spacingLeft = (baseWidget->pos.spacingLeft + ((width / slider->range) * i));
        pos.width = baseWidget->pos.spacingLeft + ((width / slider->range) * (i + 1));

        UiUitls_DrawRectangleRelative(pos);

        if (*(slider->value) == i){
            UiUtils_FillRectangleRelative(pos);
        }

    }
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

}

sliderWidget_t * initSlider(CommonPos_t pos, int * value, int range){
    sliderWidget_t * slider = (sliderWidget_t *) calloc(1, sizeof(sliderWidget_t));
    slider->baseWidget.pos = pos;
    slider->baseWidget.initPos = pos;
    slider->baseWidget.drawHandler = &drawSlider;
    slider->baseWidget.onClick = &onClickSlider;

    slider->range = range;
    slider->value = value;
    
    return slider;
}