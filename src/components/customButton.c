#include "customButton.h"

#include <assert.h>
#include <stdio.h>

#include "UiUtils.h"
#include "common.h"

static void drawButton(BaseWidget_t* baseWidget) {
    assert(baseWidget->type == WIDGET_TYPE_BUTTON);
    buttonWidget_t* button = (buttonWidget_t*)baseWidget;

    if (button->theme != NULL) {
        COLORREF fill =
            button->beingHovered ? button->theme->color.hover : button->theme->color.fill;

        UiUtils_DrawColoredRectangle(baseWidget->pos, fill, button->theme->color.border,
                                     button->theme->borderWidth);

        if (UiUtils_TextFitsBoxTheme(button->name, baseWidget->pos, button->theme->text.font)) {
            UiUtils_DrawTextTheme(baseWidget->pos, button->name, button->theme->text.formatFlags,
                                  button->theme->text.font, button->theme->text.color);
        }
    }
#ifdef DEBUG
    else {
        printf("custom button with ID: %d missing theme\n", button->id);
    }
#endif
}

static void onClickButton(BaseWidget_t* base, int x, int y, ClickType_t clickType) {
    assert(base->type == WIDGET_TYPE_BUTTON);
    buttonWidget_t* button = (buttonWidget_t*)base;

    (void)x;
    (void)y;

    switch (clickType) {
        case CLICK_TYPE_LDOWN:
            button->beingPressed = true;
            InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
            break;

        case CLICK_TYPE_LUP:
            if (button->beingPressed) {
                button->beingPressed = false;
                InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
                if (button->onClickUserCallback) {
                    button->onClickUserCallback(button->id);
                }
#ifdef DEBUG
                else {
                    printf("custom button with ID: %d missing callback\n", button->id);
                }
#endif
            }
            break;

        default:
            break;
    }
}

static void drawImageButton(BaseWidget_t* baseWidget) {
    buttonWidget_t* button = (buttonWidget_t*)baseWidget;
    AbsolutePos_t pos = baseWidget->pos;

    if (button->beingPressed) {
        int shrinkX = (pos.right - pos.left) / 20;
        int shrinkY = (pos.bottom - pos.top) / 20;
        pos.left += shrinkX;
        pos.right -= shrinkX;
        pos.top += shrinkY;
        pos.bottom -= shrinkY;
    }

    DrawIconEx(currentWindowState.memHDC, pos.left, pos.top, button->images[0],
               pos.right - pos.left, pos.bottom - pos.top, 0, NULL, DI_NORMAL);

    if (button->beingHovered && !button->beingPressed) {
        UiUtils_DrawHoverOverlay(baseWidget->pos);
    }
}

static void draw3SliceButton(BaseWidget_t* baseWidget) {
    buttonWidget_t* button = (buttonWidget_t*)baseWidget;

    AbsolutePos_t pos = baseWidget->pos;

    if (button->beingPressed) {
        int shrinkX = (pos.right - pos.left) / 20;
        int shrinkY = (pos.bottom - pos.top) / 20;
        pos.left += shrinkX;
        pos.right -= shrinkX;
        pos.top += shrinkY;
        pos.bottom -= shrinkY;
    }

    UiUtils_draw3SliceHelper(pos, button->images[0], button->images[1], button->images[2]);

    if (button->theme != NULL) {
        if (UiUtils_TextFitsBoxTheme(button->name, pos, button->theme->text.font)) {
            if (button->beingPressed) {
                HFONT smallerFont = shrinkFont(button->theme->text.font, 0.9f);

                UiUtils_DrawTextTheme(pos, button->name, button->theme->text.formatFlags,
                                      smallerFont, button->theme->text.color);

                DeleteObject(smallerFont);
            } else {
                UiUtils_DrawTextTheme(pos, button->name, button->theme->text.formatFlags,
                                      button->theme->text.font, button->theme->text.color);
            }
        }
    }
#ifdef DEBUG
    else {
        printf("custom button with ID: %d missing theme\n", button->id);
    }
#endif
    if (button->beingHovered && !button->beingPressed) {
        UiUtils_DrawHoverOverlay(baseWidget->pos);
    }
}

static void onHoverButton(BaseWidget_t* base) {
    assert(base->type == WIDGET_TYPE_BUTTON);
    buttonWidget_t* button = (buttonWidget_t*)base;

    if (!button->beingHovered) {
        button->beingHovered = true;
        InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
    }
}

static void onHoverEndButton(BaseWidget_t* base) {
    assert(base->type == WIDGET_TYPE_BUTTON);
    buttonWidget_t* button = (buttonWidget_t*)base;

    button->beingHovered = false;
    button->beingPressed = false;  // fix error, that when leaving button area while holding left
                                   // click and then releasing would leave beingPressed true
    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

static void buttonFree(BaseWidget_t* base) {
    assert(base->type == WIDGET_TYPE_BUTTON);
    buttonWidget_t* button = (buttonWidget_t*)base;
    memset(button, 0, sizeof(buttonWidget_t));
    free(button);
}

buttonWidget_t* customButton_initButton(CommonPos_t pos, void (*onClickUserCallback)(int), int id) {
    buttonWidget_t* button = (buttonWidget_t*)calloc(1, sizeof(buttonWidget_t));

    button->baseWidget.initPos = pos;

    button->baseWidget.drawHandler = &drawButton;
    button->baseWidget.onHover = &onHoverButton;
    button->baseWidget.onHoverEnd = &onHoverEndButton;
    button->baseWidget.onClick = &onClickButton;
    button->baseWidget.destroy = &buttonFree;

    button->onClickUserCallback = onClickUserCallback;

    button->id = id;
    button->baseWidget.type = WIDGET_TYPE_BUTTON;

    button->theme = &currentWindowState.activeTheme.button;

    strncpy(button->name, "Missing Name", BUTTON_MAX_NAME_LENGTH);

    return button;
}

void customButton_setButtonText(buttonWidget_t* button, const char* name) {
    if (!name) {
        button->name[0] = '\0';
        return;
    }
    strncpy(button->name, name, BUTTON_MAX_NAME_LENGTH);
    button->name[BUTTON_MAX_NAME_LENGTH - 1] = '\0';
}

void customButton_setToIcon(buttonWidget_t* button, int iconID) {
    AbsolutePos_t pos = button->baseWidget.pos;
    button->images[0] =
        (HICON)LoadImage(currentWindowState.hInstance, MAKEINTRESOURCE(iconID), IMAGE_ICON,
                         pos.right - pos.left, pos.bottom - pos.top, LR_DEFAULTCOLOR);
    button->baseWidget.drawHandler = &drawImageButton;
}

void customButton_setTo3SliceBackground(buttonWidget_t* button, int iconIDleft, int iconIDcenter,
                                        int iconIDright) {
    AbsolutePos_t pos = button->baseWidget.pos;
    button->images[0] =
        (HICON)LoadImage(currentWindowState.hInstance, MAKEINTRESOURCE(iconIDleft), IMAGE_ICON,
                         pos.right - pos.left, pos.bottom - pos.top, LR_DEFAULTCOLOR);
    button->images[1] =
        (HICON)LoadImage(currentWindowState.hInstance, MAKEINTRESOURCE(iconIDcenter), IMAGE_ICON,
                         pos.right - pos.left, pos.bottom - pos.top, LR_DEFAULTCOLOR);
    button->images[2] =
        (HICON)LoadImage(currentWindowState.hInstance, MAKEINTRESOURCE(iconIDright), IMAGE_ICON,
                         pos.right - pos.left, pos.bottom - pos.top, LR_DEFAULTCOLOR);
    button->baseWidget.drawHandler = &draw3SliceButton;
}