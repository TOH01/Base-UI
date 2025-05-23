#include "textDump.h"
#include "UiUtils.h"
#include <assert.h>
#include <stdio.h>
#ifdef CUSTOM_TITLE_BAR
#include "titlbar.h"
#endif

textDumpWidget_t *activeTextDump = NULL;
bool scrollCallbackRegisters = false;

void drawTextDump(BaseWidget_t *baseWidget) {
	assert(baseWidget->type == WIDGET_TYPE_TEXT_DUMP);
	textDumpWidget_t *textDump = (textDumpWidget_t *)baseWidget;

	int lineHeight = UiUtils_getLineHeight(textDump->theme->text.font);

	RECT containerRect = UiUtils_absolutePosToRect(baseWidget->pos);

	if (!lineHeight) {
#ifdef DEBUG
		printf("Cant draw text dump ... likely font is missing");
#endif

		return;
	}

	int visibleLines = (baseWidget->pos.bottom - baseWidget->pos.top) / lineHeight;



	if (!visibleLines) {
#ifdef DEBUG
		printf("Cant draw text dump ... visibleLines is 0, causing 0 divison error");
#endif

		return;
	}

	textLineNode_t *currentLine = textDump->currentLine;

	if (currentLine == NULL) {

		AbsolutePos_t text = baseWidget->pos;
		text.bottom = text.top + lineHeight;

		UiUtils_DrawTextTheme(text, "EMPTY TEXT DUMP ERROR", textDump->theme->text.formatFlags, textDump->theme->text.font, textDump->theme->text.color);
	} else {

		AbsolutePos_t text = baseWidget->pos;
		text.bottom = text.top + lineHeight;

		for (int i = 0; (i < visibleLines) && (currentLine != NULL); i++) {
			UiUtils_DrawTextTheme(text, currentLine->line, textDump->theme->text.formatFlags, textDump->theme->text.font, textDump->theme->text.color);
			text.top += lineHeight;
			text.bottom += lineHeight;
			currentLine = currentLine->nextNode;
		}
	}
}

void onClickTextDump(BaseWidget_t *base, int x, int y) {
	textDumpWidget_t *textDump = (textDumpWidget_t *)base;
	activeTextDump = textDump;
}

void refreshTextDump(textDumpWidget_t *textDump) { InvalidateRect(currentWindowState.hwnd, NULL, FALSE); }

void customTextDump_AddLine(textDumpWidget_t *textDump, const char *newText) {
	textLineNode_t *newNode = (textLineNode_t *)malloc(sizeof(textLineNode_t));

	newNode->line = _strdup(newText);
	newNode->nextNode = NULL;
	newNode->prevNode = NULL;

	if (!textDump->firstLine) {
		textDump->firstLine = newNode;
		textDump->currentLine = newNode;
	} else {
		textDump->lastLine->nextNode = newNode;
		newNode->prevNode = textDump->lastLine;
	}

	textDump->lastLine = newNode;
}

LRESULT scrollCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (activeTextDump) {

		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(hwnd, &mousePos);

		if (UiUtils_CoordinateIsInsideOf(mousePos.x, mousePos.y, activeTextDump->baseWidget.pos)) {

			int delta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (delta > 0) {
				if (activeTextDump->currentLine != activeTextDump->firstLine) {
					activeTextDump->currentLine = activeTextDump->currentLine->prevNode;
					InvalidateRect(hwnd, NULL, FALSE);
				}
			} else {

				if (activeTextDump->currentLine != activeTextDump->lastLine) {
					activeTextDump->currentLine = activeTextDump->currentLine->nextNode;
					InvalidateRect(hwnd, NULL, FALSE);
				}
			}
		}
	}
}

textDumpWidget_t *customTextDump_initTextDump(CommonPos_t pos) {
	textDumpWidget_t *textDump = (textDumpWidget_t *)calloc(1, sizeof(textDumpWidget_t));
	textDump->baseWidget.initPos = pos;
	textDump->baseWidget.onClick = &onClickTextDump;
	textDump->baseWidget.drawHandler = &drawTextDump;
	textDump->baseWidget.type = WIDGET_TYPE_TEXT_DUMP;
	textDump->theme = &currentWindowState.activeTheme.textDump;

	if (!scrollCallbackRegisters) {
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEWHEEL, &scrollCallback);
		scrollCallbackRegisters = true;
	}

	return textDump;
}
