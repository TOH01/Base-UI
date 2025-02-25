#include "costumCheckbox.h"
#include "common.h"
#include "widget.h"
#include "UiUtils.h"

void drawCheckbox(BaseWidget_t * baseWidget){
    
    checkboxWidget_t * checkbox = (checkboxWidget_t *) baseWidget;
    UiUtils_DrawRectangleRelative(baseWidget->pos);


    if (*(checkbox->value)){
        UiUtils_FillRectangleRelative(baseWidget->pos);
    }

}

void onClickCheckbox(BaseWidget_t * baseWidget, int x, int y){
    checkboxWidget_t * checkbox = (checkboxWidget_t *) baseWidget;
    *(checkbox->value) = !*(checkbox->value);
}

checkboxWidget_t * costumCheckbox_initCheckbox(CommonPos_t pos, bool * value){
    checkboxWidget_t * checkbox = (checkboxWidget_t *) calloc(1, sizeof(checkboxWidget_t));
    checkbox->baseWidget.pos = pos;
    checkbox->baseWidget.drawHandler = &drawCheckbox;
    checkbox->baseWidget.initPos = pos;
    checkbox->baseWidget.onClick = &onClickCheckbox;
    checkbox->value = value;
}