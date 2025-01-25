#include "common.h"
#include "costumButton.h"
#include "UiUtils.h"

void drawButton(BaseWidget_t * button){
    SelectObject(currentWindowState.memHDC, currentWindowState.hPen);
    UiUitls_DrawRectangleRelative(button->pos);
}

buttonWidget_t * initButton(CommonPos_t pos){
    buttonWidget_t * button = (buttonWidget_t *) calloc(1, sizeof(buttonWidget_t));
    BaseWidget_t * base = (BaseWidget_t *) calloc(1, sizeof(BaseWidget_t));
    button->baseWidget = base;
    button->baseWidget->drawHandler = &drawButton;
    button->baseWidget->initPos = pos; 
    button->baseWidget->pos = pos;
    return button;
}

buttonWidget_t * initButtonContainer(){

}

bool isClicked(int x, int y, BaseWidget_t * button){
    return UiUtils_CoordinateIsInsideOf(x, y, button->pos);
}