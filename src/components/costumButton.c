#include "common.h"
#include "costumButton.h"
#include "UiUtils.h"

void drawButton(BaseWidget_t * button){
    SelectObject(currentWindowState.memHDC, currentWindowState.hPen);
    UiUitls_DrawRectangleRelative(button->pos);
}

void onClick(BaseWidget_t * button){
    buttonWidget_t * buttonWidget = (buttonWidget_t *) button;
    buttonWidget->onClickUserCallback();
}

buttonWidget_t * initButton(CommonPos_t pos, void (*onClickUserCallback)(void)){
    buttonWidget_t * button = (buttonWidget_t *) calloc(1, sizeof(buttonWidget_t));
    button->baseWidget.drawHandler = &drawButton;
    button->baseWidget.initPos = pos; 
    button->baseWidget.pos = pos;
    button->baseWidget.onClick = &onClick;
    button->onClickUserCallback = onClickUserCallback;
    return button;
}

