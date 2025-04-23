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


typedef struct InputTheme {
    TextComponent_t inputText;
    TextComponent_t emptyText;
    ColorComponent_t color;
    ColorComponent_t active;
    float borderWidth;
} InputTheme_t;

typedef struct CheckboxTheme {
    ColorComponent_t inner;
    ColorComponent_t outer;
    float borderWidth;
    float spacing;
} CheckboxTheme_t;

// the slider is contained in a rectangular space. It will always fit within the size of that Rectangle. With spacings the slider can be modified.
typedef struct SliderTheme {
    ColorComponent_t thumb; //thumb color
    ColorComponent_t bar; //bar color
    float thumbBorderWidth; //width of thumb border
    float barBorderWidht; //width of bar border
    float barSpacingTop;  //space from top of slider "recatangle" where bar begins
    float barSpacingBottom;  //space from bottom of slider "recatangle" where bar ends
    float thumbSpacingTop; //space from top of slider "recatangle" where thumb begins
    float thumbSpacingBottom; //space from bottom of slider "recatangle" where thumb ends
} SliderTheme_t;

typedef struct UiTheme {
    ButtonTheme_t button;
    InputTheme_t input;
    CheckboxTheme_t checkbox;
    SliderTheme_t slider;
} UiTheme_t;

#define DEFAULT_THEME() ((UiTheme_t){ \
    .button = { \
        .color = {RGB(55,55,55), RGB(24,24,24) , RGB(44,44,44)}, \
        .text = {RGB(204,204,204), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .borderWidth = 3, \
    }, \
    .input = { \
        .inputText = {RGB(204,204,204), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .emptyText = {RGB(154,154,154), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .color = {RGB(55,55,55), RGB(24,24,24) , RGB(44,44,44)}, \
        .active = {RGB(0,120,212) ,RGB(55,55,55) , RGB(55,55,55)}, \
        .borderWidth = 3, \
    }, \
    .checkbox = { \
        .inner = {RGB(0, 120, 212), RGB(0, 120, 212) , RGB(0, 153, 255)}, \
        .outer = {RGB(55,55,55), RGB(24, 24, 24), RGB(55, 55, 55)}, \
        .borderWidth = 1, \
        .spacing = 0.15, \
    }, \
    .slider = { \
        .thumb = { RGB(0, 120, 212), RGB(50, 50, 50), RGB(0, 153, 255) }, \
        .bar = { RGB(70, 70, 70), RGB(30, 30, 30), RGB(80, 80, 80) }, \
        .thumbBorderWidth = 2.0f, \
        .barBorderWidht = 1.0f, \
        .barSpacingTop = 0.42f, \
        .barSpacingBottom = 0.42f, \
        .thumbSpacingTop = 0.1f, \
        .thumbSpacingBottom = 0.1f, \
    }, \
})

void UiTheme_initFonts();

#endif