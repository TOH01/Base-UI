#include "common.h"
#include "costumButton.h"
#include "UiUtils.h"

void drawButton(BaseWidget_t * baseWidget){
    buttonWidget_t * button = (buttonWidget_t *) baseWidget;


    COLORREF fill = button->beingHovered ? currentWindowState.activeTheme.button.color.hover : currentWindowState.activeTheme.button.color.fill;


    UiUtils_DrawColoredRectangle(baseWidget->pos,fill, currentWindowState.activeTheme.button.color.border, currentWindowState.activeTheme.button.borderWidth);

    if (UiUtils_TextFitsBoxTheme(button->name, baseWidget->pos, currentWindowState.activeTheme.button.text.font)){
        UiUtils_DrawTextTheme(baseWidget->pos, button->name, currentWindowState.activeTheme.button.text.formatFlags, currentWindowState.activeTheme.button.text.font, currentWindowState.activeTheme.button.text.color);
    }

}

void onClickButton(BaseWidget_t * base, int x, int y){
    buttonWidget_t * button = (buttonWidget_t *) base;
    button->onClickUserCallback(button->id);
}

void onHoverButton(BaseWidget_t * base){
    buttonWidget_t * button = (buttonWidget_t *) base;
    
    if(!button->beingHovered){
        button->beingHovered = 1;
        InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
    }

}

void onHoverEndButton(BaseWidget_t * base){
    buttonWidget_t * button = (buttonWidget_t *) base;
    button->beingHovered = 0;
    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

buttonWidget_t * costumButton_initButton(CommonPos_t pos, void (*onClickUserCallback)(int), int id){
    buttonWidget_t * button = (buttonWidget_t *) calloc(1, sizeof(buttonWidget_t));
    button->baseWidget.drawHandler = &drawButton;
    button->baseWidget.initPos = pos; 
    button->baseWidget.pos = pos;
    button->baseWidget.onClick = &onClickButton;
    button->onClickUserCallback = onClickUserCallback;
    button->baseWidget.onHover = &onHoverButton;
    button->baseWidget.onHoverEnd = &onHoverEndButton;
    button->id = id;
    memcpy(button->name, "", 1);
    return button;
}

void costumButton_SetButtonText(buttonWidget_t * button, char name[BUTTON_MAX_NAME_LENGTH]){
    memcpy(button->name, name, BUTTON_MAX_NAME_LENGTH);
}

