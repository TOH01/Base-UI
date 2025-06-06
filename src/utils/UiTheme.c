#include "UiUtils.h"
#include "common.h"

void UiTheme_initFonts(void) {
	currentWindowState.activeTheme.button.text.font = UiUtils_CreateFont();
	currentWindowState.activeTheme.input.inputText.font = UiUtils_CreateFont();
	currentWindowState.activeTheme.input.emptyText.font = UiUtils_CreateFont();
	currentWindowState.activeTheme.textDump.text.font = UiUtils_CreateFont();
	currentWindowState.activeTheme.label.text.font = UiUtils_CreateFont();
}