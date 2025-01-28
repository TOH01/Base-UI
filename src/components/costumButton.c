#include "common.h"
#include "costumButton.h"
#include "UiUtils.h"

void drawButton(BaseWidget_t * baseWidget){
    SelectObject(currentWindowState.memHDC, currentWindowState.hPen);
    UiUitls_DrawRectangleRelative(baseWidget->pos);
}

void onClickButton(BaseWidget_t * button, int x, int y){
    buttonWidget_t * buttonWidget = (buttonWidget_t *) button;
    buttonWidget->onClickUserCallback();
}

buttonWidget_t * initButton(CommonPos_t pos, void (*onClickUserCallback)(void)){
    buttonWidget_t * button = (buttonWidget_t *) calloc(1, sizeof(buttonWidget_t));
    button->baseWidget.drawHandler = &drawButton;
    button->baseWidget.initPos = pos; 
    button->baseWidget.pos = pos;
    button->baseWidget.onClick = &onClickButton;
    button->onClickUserCallback = onClickUserCallback;
    return button;
}

