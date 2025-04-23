#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "customButton.h"
#include "UiUtils.h"

static void drawButton(BaseWidget_t * baseWidget){
    assert(baseWidget->type == WIDGET_TYPE_BUTTON);
    buttonWidget_t * button = (buttonWidget_t *) baseWidget;
    
    if(button->theme != NULL){
        COLORREF fill = button->beingHovered ? button->theme->color.hover : button->theme->color.fill;

        UiUtils_DrawColoredRectangle(baseWidget->pos,fill, button->theme->color.border, button->theme->borderWidth);
    
        if (UiUtils_TextFitsBoxTheme(button->name, baseWidget->pos, button->theme->text.font)){
            UiUtils_DrawTextTheme(baseWidget->pos, button->name, button->theme->text.formatFlags, button->theme->text.font, button->theme->text.color);
        }
    }
    #ifdef DEBUG
    else {
        printf("custom button with ID: %d missing theme\n", button->id);
    }
    #endif

}

static void onClickButton(BaseWidget_t * base, int x, int y){
    assert(base->type == WIDGET_TYPE_BUTTON);
    buttonWidget_t * button = (buttonWidget_t *) base;
    
    if(button->onClickUserCallback != NULL){
        button->onClickUserCallback(button->id);
    }
    #ifdef DEBUG
    else {
        printf("custom button with ID: %d missing callback\n", button->id);
    }
    #endif
    
}

static void onHoverButton(BaseWidget_t * base){
    assert(base->type == WIDGET_TYPE_BUTTON);
    buttonWidget_t * button = (buttonWidget_t *) base;
    
    if(!button->beingHovered){
        button->beingHovered = true;
        InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
    }

}

static void onHoverEndButton(BaseWidget_t * base){
    assert(base->type == WIDGET_TYPE_BUTTON);
    buttonWidget_t * button = (buttonWidget_t *) base;
    
    button->beingHovered = false;
    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);

}

buttonWidget_t * customButton_initButton(CommonPos_t pos, void (*onClickUserCallback)(int), int id){
    buttonWidget_t * button = (buttonWidget_t *) calloc(1, sizeof(buttonWidget_t));
    
    button->baseWidget.initPos = pos; 
    button->baseWidget.pos = pos;

    button->baseWidget.drawHandler = &drawButton;
    button->baseWidget.onHover = &onHoverButton;
    button->baseWidget.onHoverEnd = &onHoverEndButton;
    button->baseWidget.onClick = &onClickButton;

    button->onClickUserCallback = onClickUserCallback;

    button->id = id;
    button->baseWidget.type = WIDGET_TYPE_BUTTON;

    button->theme = &currentWindowState.activeTheme.button;
    
    strncpy(button->name, "Missing Name", BUTTON_MAX_NAME_LENGTH);
    
    return button;
}

void customButton_SetButtonText(buttonWidget_t * button, const char name[BUTTON_MAX_NAME_LENGTH]){
    strncpy(button->name, name, BUTTON_MAX_NAME_LENGTH);
    button->name[BUTTON_MAX_NAME_LENGTH - 1] = '\0';
}

