#ifndef COSTUM_SLIDER_H
#define COSTUM_SLIDER_H

#include "common.h"
#include "widget.h"

typedef enum{
	NONE,
	THUMB,
	BAR,
} selectedSliderPart;

typedef struct SliderWidget {
	BaseWidget_t baseWidget;
	int *value;
	int range;
	SliderTheme_t *theme;
	int beingHovered;
	selectedSliderPart selected;
} sliderWidget_t;

sliderWidget_t *customSlider_initSlider(CommonPos_t pos, int *value, int range);

#endif