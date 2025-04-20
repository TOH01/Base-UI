#include "common.h"
#include "UiUtils.h"

void UiTheme_initFonts(){
    currentWindowState.activeTheme.button.text.font = UiUtils_CreateFont();
    currentWindowState.activeTheme.input.inputText.font = UiUtils_CreateFont();
}