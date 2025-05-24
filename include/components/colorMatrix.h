#ifndef COLOR_MATRIX_H
#define COLOR_MATRIX_H

#include "common.h"
#include "widget.h"

typedef struct colorMatrix {
	BaseWidget_t baseWidget;
    int width;
    int height;
} colorMatrix_t;

colorMatrix_t * initColorMatrix(CommonPos_t pos, int width, int height);

#endif