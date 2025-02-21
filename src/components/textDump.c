#include "textDump.h"
#include "UiUtils.h"


textDumpWidget_t * activeTextDump = NULL;

void drawTextDump(BaseWidget_t * baseWidget){
    textDumpWidget_t *  textDump = (textDumpWidget_t*) baseWidget;
    
    UiUitls_DrawText(baseWidget->pos, textDump->textBuffer, DT_WORDBREAK);
}

void onClickTextDump(BaseWidget_t * base, int x, int y){
    textDumpWidget_t *  textDump = (textDumpWidget_t*) base;
    activeTextDump = textDump;
}

void refreshTextDump(textDumpWidget_t * textDump){
    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

LRESULT scrollCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    if(activeTextDump != NULL){

    }
}

textDumpWidget_t * initTextDump(CommonPos_t pos, char * textBuffer){
    textDumpWidget_t * textDump = (textDumpWidget_t *) calloc(1, sizeof(textDumpWidget_t));
    textDump->baseWidget.pos = pos;
    textDump->baseWidget.initPos = pos;
    textDump->baseWidget.onClick = &onClickTextDump;
    textDump->baseWidget.drawHandler = &drawTextDump;

    textDump->textBuffer = textBuffer;

    return textDump;
}


