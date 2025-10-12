#include <stdio.h>
#include <string.h>

#include "UiUtils.h"
#include "assert.h"
#include "customInput.h"
#include "string.h"

bool HandlerInit = 0;
inputWidget_t* activeInput = NULL;
void (*OnActiveInputEnd)(void) = NULL;

static void drawInput(BaseWidget_t* baseWidget) {
    assert(baseWidget->type == WIDGET_TYPE_INPUT);
    inputWidget_t* input = (inputWidget_t*)baseWidget;

    if (input->theme != NULL) {
        COLORREF fill = input->beingHovered ? input->theme->color.hover : input->theme->color.fill;
        COLORREF border = (input == activeInput) ? input->theme->active.border : input->theme->color.border;

        UiUtils_DrawColoredRectangle(baseWidget->pos, fill, border, input->theme->borderWidth);

        if (strlen(input->buffer) >= 1) {
            if (UiUtils_TextFitsBoxTheme(input->buffer, baseWidget->pos, input->theme->inputText.font)) {
                UiUtils_DrawTextTheme(baseWidget->pos, input->buffer, input->theme->inputText.formatFlags, input->theme->inputText.font,
                                      input->theme->inputText.color);
            } else {
                int lastElementIdx = strlen(input->buffer) - 1;

                while (UiUtils_TextFitsBoxTheme(&input->buffer[lastElementIdx], baseWidget->pos, input->theme->inputText.font)) {
                    lastElementIdx--;
                }

                if (lastElementIdx >= 0) {
                    UiUtils_DrawTextTheme(baseWidget->pos, &input->buffer[lastElementIdx], input->theme->inputText.formatFlags,
                                          input->theme->inputText.font, input->theme->inputText.color);
                }
#ifdef DEBUG
                else {
                    printf("text cannot fit input\n");
                }
#endif
            }
        } else {
            UiUtils_DrawTextTheme(baseWidget->pos, input->defaultText, input->theme->emptyText.formatFlags, input->theme->emptyText.font,
                                  input->theme->emptyText.color);
        }
    }
#ifdef DEBUG
    else {
        printf("input missing theme\n");
    }
#endif
}

static void onHoverInput(BaseWidget_t* base) {
    assert(base->type == WIDGET_TYPE_INPUT);
    inputWidget_t* input = (inputWidget_t*)base;

    if (!input->beingHovered) {
        input->beingHovered = 1;
        InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
    }
}

static void onHoverEndInput(BaseWidget_t* base) {
    assert(base->type == WIDGET_TYPE_INPUT);
    inputWidget_t* input = (inputWidget_t*)base;
    input->beingHovered = 0;
    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

void setOnActiveInputEndCbk(void* end) { OnActiveInputEnd = end; }

void endActiveInput(void) {
    if (activeInput != NULL) {
        activeInput = NULL;
        if (OnActiveInputEnd != NULL) {
            OnActiveInputEnd();
        }
    }
}

static LRESULT keystoreCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    (void)hwnd;
    (void)msg;
    (void)lParam;

    if (activeInput) {
        int buffLen = strlen(activeInput->buffer);

        if (wParam == VK_ESCAPE) {
            endActiveInput();
        }

        else if (wParam == VK_DELETE || wParam == VK_BACK) {
            if (buffLen >= 1) {
                activeInput->buffer[buffLen - 1] = '\0';
                if (activeInput->type == INPUT_MODE_NUMERIC) {
                    char* endptr;
                    long val = strtol(activeInput->buffer, &endptr, 10);
                    if (*endptr == '\0') {  // entire string was a valid number
                        if (val > INT_MAX) {
                            val = INT_MAX;
                        }
                        if (val < INT_MIN) {
                            val = INT_MIN;
                        }
                        *activeInput->value = (int)val;
                    }
                }
                if (activeInput->onDataChange != NULL) {
                    activeInput->onDataChange();
                }
            }
        } else if (buffLen < INPUT_MAX_STRING_SIZE - 1 && wParam >= 32 && wParam <= 126) {
            activeInput->buffer[buffLen] = (char)wParam;
            activeInput->buffer[buffLen + 1] = '\0';
            if (activeInput->type == INPUT_MODE_NUMERIC) {
                char* endptr;
                long val = strtol(activeInput->buffer, &endptr, 10);
                if (*endptr == '\0') {  // entire string was a valid number
                    if (val > INT_MAX) {
                        val = INT_MAX;
                    }
                    if (val < INT_MIN) {
                        val = INT_MIN;
                    }
                    *activeInput->value = (int)val;
                }
            }
            if (activeInput->onDataChange != NULL) {
                activeInput->onDataChange();
            }
        }

        // trigger redraw to show new letters
        InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
    }

    return 0;
}

/**
 * @brief click callbkac for input, if its clicked set it so active
 *
 * @param baseWidget : pointer to base widget of input
 * @param x : mouse position x
 * @param y : mouse position y
 * @param clickType : click type
 */
static void onClickInput(BaseWidget_t* baseWidget, int x, int y, ClickType_t clickType) {
    assert(baseWidget->type == WIDGET_TYPE_INPUT);
    inputWidget_t* input = (inputWidget_t*)baseWidget;
    (void)x;
    (void)y;
    (void)clickType;
    activeInput = input;
}

/**
 * @brief frees the memory of the input belonging to passed
 *        base widget. If that inpuct is activeInput, reset
 *        it.
 *
 * @param baseWidget : pointer to base widget of input
 */
static void CustomInput_freeInput(BaseWidget_t* baseWidget) {
    assert(baseWidget->type == WIDGET_TYPE_INPUT);
    inputWidget_t* input = (inputWidget_t*)baseWidget;
    if (input == activeInput) {
        activeInput = NULL;
    }
    memset(input, 0, sizeof(inputWidget_t));
    free(input);
}

inputWidget_t* customInput_initInput(CommonPos_t pos) {
    inputWidget_t* input = (inputWidget_t*)calloc(1, sizeof(inputWidget_t));

    if (!HandlerInit) {
        WmParamTable_Insert(currentWindowState.handlerTable, WM_KEYDOWN, &keystoreCallback);
        HandlerInit = 1;
    }

    input->baseWidget.initPos = pos;

    input->baseWidget.type = WIDGET_TYPE_INPUT;

    input->theme = &currentWindowState.activeTheme.input;
    input->type = INPUT_MODE_TEXT;

    input->baseWidget.drawHandler = &drawInput;
    input->baseWidget.onClick = &onClickInput;
    input->baseWidget.onHover = &onHoverInput;
    input->baseWidget.onHoverEnd = &onHoverEndInput;
    input->baseWidget.destroy = &CustomInput_freeInput;

    strncpy(input->defaultText, "Input ...", INPUT_MAX_DEFAULT_STRING_SIZE);

    return input;
}

inputWidget_t* customInput_initInputNumeric(CommonPos_t pos, int* value) {
    inputWidget_t* input = customInput_initInput(pos);

    input->type = INPUT_MODE_NUMERIC;
    input->value = value;

    if (value) {
        snprintf(input->buffer, sizeof(input->buffer), "%d", *value);
    }

    return input;
}

void customInput_clearInput(inputWidget_t* input) { input->buffer[0] = '\0'; }

void setDefaultText(inputWidget_t* input, char* defaultText) {
    strncpy(input->defaultText, defaultText, INPUT_MAX_DEFAULT_STRING_SIZE);
    input->defaultText[INPUT_MAX_DEFAULT_STRING_SIZE - 1] = '\0';
}
