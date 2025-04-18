#ifndef UI_THEME_H
#define UI_THEME_H

#include "windows.h"

typedef struct ColorComponent {
    COLORREF border;
    COLORREF fill;
} ColorComponent_t;

typedef struct SpacingComponent {

} SpacingComponent_t;


typedef struct ButtonTheme {
    ColorComponent_t color;
} ButtonTheme_t;


typedef struct UiTheme {
    ButtonTheme_t button;
} UiTheme_t;

#define DEFAULT_THEME() ((UiTheme_t){ \
    .button = { \
        .color = {RGB(255, 255, 0), RGB(0, 255, 255)}, \
    } \
})

#endif