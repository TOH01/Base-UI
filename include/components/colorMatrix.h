#ifndef COLOR_MATRIX_H
#define COLOR_MATRIX_H

#include "common.h"
#include "widget.h"

typedef struct colorMatrix {
	BaseWidget_t baseWidget;
    int width;
    int height;
    int activeRow;
    int activeColumn;
} colorMatrix_t;

colorMatrix_t * initColorMatrix(CommonPos_t pos, int width, int height);
COLORREF colorMatrixGetActive(colorMatrix_t *colorMatrix);
bool colorMatrixHasActive(colorMatrix_t *colorMatrix);

#endif