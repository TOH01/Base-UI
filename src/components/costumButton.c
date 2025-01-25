#include "common.h"
#include "costumButton.h"
#include "UiUtils.h"

buttonWidget_t * initButton(WmParamHandlerTable_t * handlerTable){

}

buttonWidget_t * initButtonContainer(){

}

void drawButton(buttonWidget_t * button){
    RoundRect(currentWindowState.memHDC, button->pos.spacingLeft, button->pos.spacingTop, button->pos.width, button->pos.height, 3, 3);
}

bool isClicked(int x, int y, buttonWidget_t * button){
    return UiUtils_CoordinateIsInsideOf(x, y, button->pos);
}