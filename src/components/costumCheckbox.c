#include <stdio.h>
#include <assert.h>
#include "costumCheckbox.h"
#include "common.h"
#include "widget.h"
#include "UiUtils.h"

static void drawCheckbox(BaseWidget_t * baseWidget){
    assert(baseWidget->type == WIDGET_TYPE_CHECKBOX);
    checkboxWidget_t * checkbox = (checkboxWidget_t *) baseWidget;
    
    if(checkbox->theme){
        UiUtils_DrawColoredRectangle(baseWidget->pos, checkbox->theme->outer.fill, checkbox->theme->outer.border, checkbox->theme->borderWidth);

        if (*(checkbox->value)){
            
            CommonPos_t inner = baseWidget->pos;
            
            float height = inner.bottom - inner.top;
            float width = inner.right - inner.left;

            inner.bottom = inner.bottom - (height * checkbox->theme->spacing);
            inner.top = inner.top + (height * checkbox->theme->spacing);
            inner.right = inner.right - (width * checkbox->theme->spacing);
            inner.left = inner.left + (width * checkbox->theme->spacing);
            
            UiUtils_DrawColoredRectangle(inner, checkbox->theme->inner.fill, checkbox->theme->inner.border, checkbox->theme->borderWidth);
        }
    }
    #ifdef DEBUG
    else {
        printf("checkbox missing theme\n");
    }
    #endif
}

static void onClickCheckbox(BaseWidget_t * baseWidget, int x, int y){
    assert(baseWidget->type == WIDGET_TYPE_CHECKBOX);
    checkboxWidget_t * checkbox = (checkboxWidget_t *) baseWidget;
    *(checkbox->value) = !*(checkbox->value);
}

checkboxWidget_t * costumCheckbox_initCheckbox(CommonPos_t pos, bool * value){
    checkboxWidget_t * checkbox = (checkboxWidget_t *) calloc(1, sizeof(checkboxWidget_t));
    
    checkbox->baseWidget.pos = pos;
    checkbox->baseWidget.initPos = pos;

    checkbox->baseWidget.type = WIDGET_TYPE_CHECKBOX;

    checkbox->baseWidget.drawHandler = &drawCheckbox;
    checkbox->baseWidget.onClick = &onClickCheckbox;
    
    checkbox->theme = &currentWindowState.activeTheme.checkbox;

    checkbox->value = value;
}