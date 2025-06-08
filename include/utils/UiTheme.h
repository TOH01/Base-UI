#ifndef UI_THEME_H
#define UI_THEME_H

#include "windows.h"
#include "config.h"

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

typedef struct textDumpTheme {
    TextComponent_t text;
} textDumpTheme_t;

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

typedef struct ContainerTheme {
    ColorComponent_t color;
    int borderWidth;
} containerTheme_t;

typedef struct labelTheme{
    TextComponent_t text;
} labelTheme_t;

typedef struct rectangleTheme{
    ColorComponent_t color;
    int borderWidth;
} rectangleTheme_t;

typedef struct lineTheme{
    ColorComponent_t color;
    int width;
} lineTheme_t;

#ifdef CUSTOM_TITLE_BAR
typedef struct titleBarTheme{
    COLORREF color;
} titleBarTheme_t;

typedef struct TitlerbarButtonTheme{
    COLORREF hover;
    COLORREF ICON;
} TitlerbarButtonTheme_t;
#endif

typedef struct UiTheme {
    ButtonTheme_t button;
    InputTheme_t input;
    CheckboxTheme_t checkbox;
    SliderTheme_t slider;
    containerTheme_t container;
    textDumpTheme_t textDump;
    labelTheme_t label;
    rectangleTheme_t rectangel;
    lineTheme_t line;
    #ifdef CUSTOM_TITLE_BAR
    titleBarTheme_t titlebar;
    TitlerbarButtonTheme_t close;
    TitlerbarButtonTheme_t restore;
    TitlerbarButtonTheme_t minimize;
    #endif
    COLORREF backgroundColor;
} UiTheme_t;


#define DARKMODE_THEME() ((UiTheme_t){ \
    .backgroundColor = RGB(30, 30, 30),\
    .button = { \
        .color = {RGB(60, 60, 60), RGB(37, 37, 38), RGB(75, 75, 75)}, \
        .text = {RGB(220, 220, 220), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .borderWidth = 2.0f, \
    }, \
    .input = { \
        .inputText = {RGB(220, 220, 220), NULL, DT_LEFT | DT_VCENTER | DT_NOCLIP}, \
        .emptyText = {RGB(128, 128, 128), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .color = {RGB(60, 60, 60), RGB(37, 37, 38), RGB(75, 75, 75)}, \
        .active = {RGB(0, 122, 204), RGB(37, 37, 38), RGB(60, 60, 60)}, \
        .borderWidth = 2.0f, \
    }, \
    .checkbox = { \
        .inner = {RGB(0, 122, 204), RGB(0, 122, 204), RGB(0, 153, 255)}, \
        .outer = {RGB(80, 80, 80), RGB(37, 37, 38), RGB(80, 80, 80)}, \
        .borderWidth = 1, \
        .spacing = 0.2f, \
    }, \
    .slider = { \
        .thumb = {RGB(0, 122, 204), RGB(45, 45, 45), RGB(0, 153, 255)}, \
        .bar = {RGB(70, 70, 70), RGB(30, 30, 30), RGB(90, 90, 90)}, \
        .thumbBorderWidth = 2, \
        .barBorderWidht = 1.0f, \
        .barSpacingTop = 0.4f, \
        .barSpacingBottom = 0.4f, \
        .thumbSpacingTop = 0.1f, \
        .thumbSpacingBottom = 0.1f, \
    }, \
    .container = { \
        .color = {RGB(70, 70, 70), RGB(30, 30, 30), RGB(90, 90, 90)}, \
        .borderWidth = 2, \
    }, \
    .textDump = { \
        .text = {RGB(220, 220, 220), NULL, DT_LEFT | DT_SINGLELINE | DT_NOCLIP}, \
    }, \
    .label = { \
        .text = {RGB(200, 200, 200), NULL, DT_LEFT | DT_VCENTER | DT_NOCLIP}, \
    }, \
    .rectangel = { \
        .color = {RGB(80, 80, 80), RGB(37, 37, 38), RGB(90, 90, 90)}, \
        .borderWidth = 1, \
    }, \
    .line = { \
        .color = {RGB(100, 100, 100), RGB(100, 100, 100), RGB(120, 120, 120)}, \
        .width = 1, \
    } \
    /* Note: no comma here, avoid trailing comma before #endif */ \
    \
    /* Titlebar customization below */ \
    IFDEF_TITLEBAR_DARKMODE \
})

#ifdef CUSTOM_TITLE_BAR
#define IFDEF_TITLEBAR_DARKMODE \
    , .titlebar = { RGB(37, 37, 38) }, \
    .close = { RGB(232, 17, 35), RGB(255, 255, 255) }, \
    .restore = { RGB(70, 70, 70), RGB(255, 255, 255) }, \
    .minimize = { RGB(70, 70, 70), RGB(255, 255, 255) }
#else
#define IFDEF_TITLEBAR_DARKMODE
#endif

#define LIGHTMODE_THEME() ((UiTheme_t){ \
    .backgroundColor = RGB(245, 245, 245),\
    .button = { \
        .color = {RGB(200, 200, 200), RGB(240, 240, 240), RGB(220, 220, 220)}, \
        .text = {RGB(30, 30, 30), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .borderWidth = 2, \
    }, \
    .input = { \
        .inputText = {RGB(30, 30, 30), NULL, DT_LEFT | DT_VCENTER | DT_NOCLIP}, \
        .emptyText = {RGB(150, 150, 150), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .color = {RGB(220, 220, 220), RGB(255, 255, 255), RGB(235, 235, 235)}, \
        .active = {RGB(0, 120, 212), RGB(255, 255, 255), RGB(235, 235, 235)}, \
        .borderWidth = 2, \
    }, \
    .checkbox = { \
        .inner = {RGB(0, 120, 212), RGB(0, 120, 212), RGB(0, 153, 255)}, \
        .outer = {RGB(180, 180, 180), RGB(240, 240, 240), RGB(160, 160, 160)}, \
        .borderWidth = 1, \
        .spacing = 0.2f, \
    }, \
    .slider = { \
        .thumb = {RGB(0, 120, 212), RGB(230, 230, 230), RGB(0, 153, 255)}, \
        .bar = {RGB(200, 200, 200), RGB(245, 245, 245), RGB(180, 180, 180)}, \
        .thumbBorderWidth = 1.5f, \
        .barBorderWidht = 1.0f, \
        .barSpacingTop = 0.4f, \
        .barSpacingBottom = 0.4f, \
        .thumbSpacingTop = 0.1f, \
        .thumbSpacingBottom = 0.1f, \
    }, \
    .container = { \
        .color = {RGB(200, 200, 200), RGB(245, 245, 245), RGB(220, 220, 220)}, \
        .borderWidth = 2, \
    }, \
    .textDump = { \
        .text = {RGB(30, 30, 30), NULL, DT_LEFT | DT_SINGLELINE | DT_NOCLIP}, \
    }, \
    .label = { \
        .text = {RGB(50, 50, 50), NULL, DT_LEFT | DT_VCENTER | DT_NOCLIP}, \
    }, \
    .rectangel = { \
        .color = {RGB(210, 210, 210), RGB(245, 245, 245), RGB(225, 225, 225)}, \
        .borderWidth = 1, \
    }, \
    .line = { \
        .color = {RGB(180, 180, 180), RGB(180, 180, 180), RGB(200, 200, 200)}, \
        .width = 1, \
    } \
    /* Note: no comma here, avoid trailing comma before #endif */ \
    \
    /* Titlebar customization below */ \
    IFDEF_TITLEBAR_LIGHTMODE \
})

#ifdef CUSTOM_TITLE_BAR
#define IFDEF_TITLEBAR_LIGHTMODE \
    , .titlebar = { RGB(255, 255, 255) }, \
    .close = { RGB(232, 17, 35), RGB(0, 0, 0) }, \
    .restore = { RGB(200, 200, 200), RGB(0, 0, 0) }, \
    .minimize = { RGB(200, 200, 200), RGB(0, 0, 0) }
#else
#define IFDEF_TITLEBAR_LIGHTMODE
#endif

#define OCEAN_BREEZE_THEME() ((UiTheme_t){ \
    .backgroundColor = RGB(245, 250, 255),\
    .button = { \
        .color = {RGB(45, 156, 219), RGB(230, 245, 250), RGB(60, 170, 230)}, \
        .text = {RGB(15, 32, 39), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .borderWidth = 2.0f, \
    }, \
    .input = { \
        .inputText = {RGB(20, 20, 20), NULL, DT_LEFT | DT_VCENTER | DT_NOCLIP}, \
        .emptyText = {RGB(130, 130, 130), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .color = {RGB(200, 230, 245), RGB(255, 255, 255), RGB(220, 240, 250)}, \
        .active = {RGB(45, 156, 219), RGB(255, 255, 255), RGB(230, 245, 255)}, \
        .borderWidth = 2.0f, \
    }, \
    .checkbox = { \
        .inner = {RGB(45, 156, 219), RGB(45, 156, 219), RGB(70, 180, 240)}, \
        .outer = {RGB(160, 200, 220), RGB(255, 255, 255), RGB(160, 200, 220)}, \
        .borderWidth = 1, \
        .spacing = 0.18f, \
    }, \
    .slider = { \
        .thumb = {RGB(45, 156, 219), RGB(240, 250, 255), RGB(70, 180, 240)}, \
        .bar = {RGB(180, 220, 235), RGB(235, 245, 250), RGB(160, 210, 230)}, \
        .thumbBorderWidth = 1.5f, \
        .barBorderWidht = 1.0f, \
        .barSpacingTop = 0.35f, \
        .barSpacingBottom = 0.35f, \
        .thumbSpacingTop = 0.12f, \
        .thumbSpacingBottom = 0.12f, \
    }, \
    .container = { \
        .color = {RGB(170, 210, 225), RGB(245, 250, 255), RGB(200, 230, 245)}, \
        .borderWidth = 2, \
    }, \
    .textDump = { \
        .text = {RGB(20, 20, 20), NULL, DT_LEFT | DT_SINGLELINE | DT_NOCLIP}, \
    }, \
    .label = { \
        .text = {RGB(30, 30, 30), NULL, DT_LEFT | DT_VCENTER | DT_NOCLIP}, \
    }, \
    .rectangel = { \
        .color = {RGB(160, 210, 230), RGB(245, 250, 255), RGB(180, 220, 235)}, \
        .borderWidth = 1, \
    }, \
    .line = { \
        .color = {RGB(140, 190, 210), RGB(140, 190, 210), RGB(160, 210, 230)}, \
        .width = 1, \
    } \
        /* Note: no comma here, avoid trailing comma before #endif */ \
        \
        /* Titlebar customization below */ \
        IFDEF_TITLEBAR_OCEAN \
})

#ifdef CUSTOM_TITLE_BAR
#define IFDEF_TITLEBAR_OCEAN \
    , .titlebar = { RGB(230, 245, 250) }, \
    .close = { RGB(232, 17, 35), RGB(15, 32, 39) }, \
    .restore = { RGB(100, 180, 210), RGB(15, 32, 39) }, \
    .minimize = { RGB(100, 180, 210), RGB(15, 32, 39) }
#else
#define IFDEF_TITLEBAR_OCEAN
#endif

#define CASTLE_CLASH_THEME() ((UiTheme_t){ \
    .backgroundColor = RGB(214, 186, 148),\
    .button = { \
        .color = {RGB(66, 78, 126), RGB(107, 125, 197), RGB(130, 150, 220)}, \
        .text = {RGB(247, 247, 255), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE}, \
        .borderWidth = 2.0f, \
    }, \
    .input = { \
        .inputText = {RGB(238, 219, 197), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .emptyText = {RGB(192, 168, 136), NULL, DT_CENTER | DT_VCENTER | DT_NOCLIP}, \
        .color = {RGB(115, 85, 58), RGB(115, 85, 58), RGB(140, 110, 80)}, \
        .active = {RGB(107, 125, 197), RGB(115, 85, 58), RGB(66, 78, 126)}, \
        .borderWidth = 2.0f, \
    }, \
    .checkbox = { \
        .inner = {RGB(107, 125, 197), RGB(107, 125, 197), RGB(130, 150, 220)}, \
        .outer = {RGB(115, 85, 58), RGB(214, 186, 148), RGB(140, 110, 80)}, \
        .borderWidth = 1, \
        .spacing = 0.2f, \
    }, \
    .slider = { \
        .thumb = {RGB(107, 125, 197), RGB(214, 186, 148), RGB(130, 150, 220)}, \
        .bar = {RGB(156, 130, 102), RGB(214, 186, 148), RGB(178, 152, 124)}, \
        .thumbBorderWidth = 2, \
        .barBorderWidht = 1.0f, \
        .barSpacingTop = 0.4f, \
        .barSpacingBottom = 0.4f, \
        .thumbSpacingTop = 0.1f, \
        .thumbSpacingBottom = 0.1f, \
    }, \
    .container = { \
        .color = {RGB(115, 85, 58), RGB(214, 186, 148), RGB(140, 110, 80)}, \
        .borderWidth = 2, \
    }, \
    .textDump = { \
        .text = {RGB(123, 85, 58), NULL, DT_LEFT | DT_SINGLELINE | DT_NOCLIP}, \
    }, \
    .label = { \
        .text = {RGB(123, 85, 58), NULL, DT_LEFT | DT_VCENTER | DT_NOCLIP}, \
    }, \
    .rectangel = { \
        .color = {RGB(140, 110, 80), RGB(214, 186, 148), RGB(156, 130, 102)}, \
        .borderWidth = 1, \
    }, \
    .line = { \
        .color = {RGB(156, 130, 102), RGB(156, 130, 102), RGB(178, 152, 124)}, \
        .width = 1, \
    } \
    /* Titlebar customization below */ \
    IFDEF_TITLEBAR_CASTLECLASH \
})

#ifdef CUSTOM_TITLE_BAR
#define IFDEF_TITLEBAR_CASTLECLASH \
    , .titlebar = {RGB(156, 130, 102)}, \
    .close = {RGB(180, 50, 50), RGB(255, 255, 255)}, \
    .restore = {RGB(123, 85, 58), RGB(255, 255, 255)}, \
    .minimize = {RGB(123, 85, 58), RGB(255, 255, 255)}
#else
#define IFDEF_TITLEBAR_CASTLECLASH
#endif

void UiTheme_initFonts(void);

#endif