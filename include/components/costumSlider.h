#ifndef COSTUM_SLIDER_H
#define COSTUM_SLIDER_H

#include "common.h"
#include "widget.h"

typedef struct SliderWidget {
    BaseWidget_t baseWidget;
    int * value;
    int range;
    SliderTheme_t * theme;
} sliderWidget_t;

sliderWidget_t * costumSlider_initSlider(CommonPos_t pos, int * value, int range);

#endif