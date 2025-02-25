#include "textDump.h"
#include "UiUtils.h"


textDumpWidget_t * activeTextDump = NULL;
bool scrollCallbackRegisters = false;

void drawTextDump(BaseWidget_t * baseWidget){
    textDumpWidget_t *  textDump = (textDumpWidget_t*) baseWidget;

    TEXTMETRIC tm;
    GetTextMetrics(currentWindowState.hdc, &tm);
    int lineHeight = tm.tmHeight;

    RECT containerRect = UiUtils_CommonPosToRect(baseWidget->pos);

    int visibleLines = (containerRect.bottom - containerRect.top) / lineHeight;  

    int y = containerRect.top;

    textLineNode_t * currentLine = textDump->currentLine;

    if(currentLine == NULL){
        UiUtils_DrawText(baseWidget->pos, "EMPTY TEXT DUMP ERROR", DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
        return;
    }

    for (int i = 0; (i < visibleLines) && (currentLine != NULL); i++) {
        
        RECT textRect = {containerRect.left, y, containerRect.right, y + lineHeight};
        UiUtils_DrawText(UiUtils_RectToCommonsPos(textRect), currentLine->line, DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
        y += lineHeight;
        currentLine = currentLine->nextNode;
    }
    
}

void onClickTextDump(BaseWidget_t * base, int x, int y){
    textDumpWidget_t *  textDump = (textDumpWidget_t*) base;
    activeTextDump = textDump;
}

void refreshTextDump(textDumpWidget_t * textDump){
    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

void costumTextDump_AddLine(textDumpWidget_t * textDump, const char * newText){
    textLineNode_t * newNode = (textLineNode_t *)malloc(sizeof(textLineNode_t));
    
    newNode->line = _strdup(newText);
    newNode->nextNode = NULL;
    newNode->prevNode = NULL;

    if (!textDump->firstLine) {
        textDump->firstLine = newNode;
        textDump->currentLine = newNode;
    } 
    else {
        textDump->lastLine->nextNode = newNode;
        newNode->prevNode = textDump->lastLine;
    }

    textDump->lastLine = newNode;

}

LRESULT scrollCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   
    if(activeTextDump){
        
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hwnd, &mousePos); 
            
        if(UiUtils_CoordinateIsInsideOf(mousePos.x, mousePos.y, activeTextDump->baseWidget.pos)) {
        
        
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            if(delta > 0){
                if(activeTextDump->currentLine != activeTextDump->firstLine){
                    activeTextDump->currentLine = activeTextDump->currentLine->prevNode;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            else{
                
                if(activeTextDump->currentLine != activeTextDump->lastLine){
                    activeTextDump->currentLine = activeTextDump->currentLine->nextNode;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
        }
    }
}    
    


textDumpWidget_t * costumTextDump_initTextDump(CommonPos_t pos){
    textDumpWidget_t * textDump = (textDumpWidget_t *) calloc(1, sizeof(textDumpWidget_t));
    textDump->baseWidget.pos = pos;
    textDump->baseWidget.initPos = pos;
    textDump->baseWidget.onClick = &onClickTextDump;
    textDump->baseWidget.drawHandler = &drawTextDump;

    if(!scrollCallbackRegisters){
        WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEWHEEL, &scrollCallback);
        scrollCallbackRegisters = true;
    }

    return textDump;
}


