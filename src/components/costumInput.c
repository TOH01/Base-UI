#include "costumInput.h"
#include "UiUtils.h"

bool handlerInit = 0;
inputWidget_t * activeInput = NULL;


void drawInput(BaseWidget_t * baseWidget){
    inputWidget_t * input = (inputWidget_t*) baseWidget;
    
    SelectObject(currentWindowState.memHDC, currentWindowState.hPen);

    UiUitls_DrawRectangleRelative(baseWidget->pos);
    

    UiUitls_DrawText(baseWidget->pos, input->buffer);
 
}

LRESULT keystoreCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

    if(activeInput){
        
        int buffLen = strlen(activeInput->buffer);


        if(wParam == VK_ESCAPE){
            activeInput = NULL;
            return 1;
        }

        if (buffLen < 255 && wParam >= 32 && wParam <= 126){
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



inputWidget_t * initInput(CommonPos_t pos){
    inputWidget_t * input = (inputWidget_t *) calloc(1, sizeof(inputWidget_t));

    if(!handlerInit){
        WmParamHanderTable_Insert(currentWindowState.wmParamHashTable, WM_KEYDOWN, &keystoreCallback);
    }

    input->baseWidget.pos = pos;
    input->baseWidget.drawHandler = &drawInput;
    input->baseWidget.initPos = pos;
    input->baseWidget.onClick = &onClickInput;

    return input;
}