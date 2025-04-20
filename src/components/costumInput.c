#include "costumInput.h"
#include "UiUtils.h"
#include "string.h"

bool handlerInit = 0;
inputWidget_t * activeInput = NULL;


void drawInput(BaseWidget_t * baseWidget){
    inputWidget_t * input = (inputWidget_t*) baseWidget;
    
    COLORREF fill = input->beingHovered ? currentWindowState.activeTheme.input.color.hover : currentWindowState.activeTheme.input.color.fill;
    COLORREF border = (input == activeInput) ? currentWindowState.activeTheme.input.active.border : currentWindowState.activeTheme.input.color.border;

    UiUtils_DrawColoredRectangle(baseWidget->pos, fill, border, currentWindowState.activeTheme.input.borderWidth);
    

    if (strlen(input->buffer)){
        if(UiUtils_TextFitsBox(input->buffer, baseWidget->pos)){
            UiUtils_DrawTextTheme(baseWidget->pos, input->buffer, currentWindowState.activeTheme.input.inputText.formatFlags, currentWindowState.activeTheme.input.inputText.font, currentWindowState.activeTheme.input.inputText.color);
        }
        else {
            int lastElementIdx = strlen(input->buffer) - 1;
    
            while(UiUtils_TextFitsBox(&input->buffer[lastElementIdx], baseWidget->pos)){
                lastElementIdx--;
            }
    
            UiUtils_DrawTextTheme(baseWidget->pos, &input->buffer[lastElementIdx], currentWindowState.activeTheme.input.inputText.formatFlags, currentWindowState.activeTheme.input.inputText.font, currentWindowState.activeTheme.input.inputText.color);
    
        }
    }
    else {
        UiUtils_DrawTextTheme(baseWidget->pos, input->defaultText, currentWindowState.activeTheme.input.emptyText.formatFlags, currentWindowState.activeTheme.input.emptyText.font, currentWindowState.activeTheme.input.emptyText.color);
    }

}

void onHoverInput(BaseWidget_t * base){
    inputWidget_t * input = (inputWidget_t *) base;
    
    if(!input->beingHovered){
        input->beingHovered = 1;
        InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
    }

}

void onHoverEndInput(BaseWidget_t * base){
    inputWidget_t * input = (inputWidget_t *) base;
    input->beingHovered = 0;
    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

LRESULT keystoreCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

    if(activeInput){

        int buffLen = strlen(activeInput->buffer);


        if(wParam == VK_ESCAPE){
            activeInput = NULL;
            return 1;
        }

        if(wParam == VK_DELETE || wParam  == VK_BACK){
            if(buffLen){
                activeInput->buffer[buffLen - 1] = '\0';
                InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
            }
                
            return 1;
        }

        if (buffLen < INPUT_MAX_STRING_SIZE - 1 && wParam >= 32 && wParam <= 126){
            activeInput->buffer[buffLen] = (char) wParam;
            activeInput->buffer[buffLen + 1] = '\0';
        }
            
            // trigger redraw to show new letters
        InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
    
        }
    
}

void onClickInput(BaseWidget_t * baseWidget, int x, int y){
    inputWidget_t * input = (inputWidget_t*) baseWidget;
    activeInput = input;
}

void costumInput_clearInput(inputWidget_t * input){
    strcpy(input->buffer, "");
}

inputWidget_t * costumInput_initInput(CommonPos_t pos){
    inputWidget_t * input = (inputWidget_t *) calloc(1, sizeof(inputWidget_t));

    if(!handlerInit){
        WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_KEYDOWN, &keystoreCallback);
    }

    input->baseWidget.pos = pos;
    input->baseWidget.drawHandler = &drawInput;
    input->baseWidget.initPos = pos;
    input->baseWidget.onClick = &onClickInput;
    input->baseWidget.onHover = &onHoverInput;
    input->baseWidget.onHoverEnd = &onHoverEndInput;

    strcpy(input->defaultText, "Input ...");

    return input;
}