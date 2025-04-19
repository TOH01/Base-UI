#ifndef UI_THEME_H
#define UI_THEME_H

#include "windows.h"

typedef struct ColorComponent {
    COLORREF border;
    COLORREF fill;
    COLORREF hover;
} ColorComponent_t;

typedef struct TextComponent {
    COLORREF color;
    HFONT font;
    UINT formatFlags;
} TextComponent_t;

typedef struct ButtonTheme {
    ColorComponent_t color;
    TextComponent_t text;
    float borderWidth;
} ButtonTheme_t;


typedef struct UiTheme {
    ButtonTheme_t button;
} UiTheme_t;

#define DEFAULT_THEME() ((UiTheme_t){ \
    .button = { \
        .color = {RGB(55,55,55), RGB(24,24,24) , RGB(44,44,44)}, \
        .text = {RGB(204,204,204), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .borderWidth = 3, \
    } \
})

void UiTheme_initFonts();

#endif