#include "common.h"
#include "costumButton.h"
#include "UiUtils.h"

void drawButton(BaseWidget_t * baseWidget){
    buttonWidget_t * button = (buttonWidget_t *) baseWidget;

    SelectObject(currentWindowState.memHDC, currentWindowState.hPen);

    UiUitls_DrawRectangleRelative(baseWidget->pos);
    
    if (UiUtils_TextFitsBox(button->name, baseWidget->pos)){
        UiUitls_DrawText(baseWidget->pos, button->name, DT_CENTER | DT_VCENTER | DT_NOCLIP);
    }

}

void onClickButton(BaseWidget_t * base, int x, int y){
    buttonWidget_t * button = (buttonWidget_t *) base;
    button->onClickUserCallback(button->id);
}

buttonWidget_t * initButton(CommonPos_t pos, void (*onClickUserCallback)(int), int id){
    buttonWidget_t * button = (buttonWidget_t *) calloc(1, sizeof(buttonWidget_t));
    button->baseWidget.drawHandler = &drawButton;
    button->baseWidget.initPos = pos; 
    button->baseWidget.pos = pos;
    button->baseWidget.onClick = &onClickButton;
    button->onClickUserCallback = onClickUserCallback;
    button->id = id;
    memcpy(button->name, "", 1);
    return button;
}

void buttonSetText(buttonWidget_t * button, char name[BUTTON_MAX_NAME_LENGTH]){
    memcpy(button->name, name, BUTTON_MAX_NAME_LENGTH);
}

