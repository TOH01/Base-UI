#include "colorMatrix.h"
#include "UiUtils.h"

COLORREF colormap[5][5] = {
    { RGB(179,  83,  83),  RGB(102, 153, 102),  RGB(102, 122, 179),  RGB(204, 170, 102),  RGB(102, 170, 170) },
    { RGB(153, 102, 102),  RGB( 90,  90, 153),  RGB(102, 170, 153),  RGB(179, 153, 102),  RGB(153, 179, 204) },
    { RGB(204, 136, 136),  RGB(136, 102, 204),  RGB( 90, 179, 153),  RGB(204, 204, 102),  RGB(136, 153, 153) },
    { RGB(204, 179, 179),  RGB(119, 102, 204),  RGB(102, 153, 170),  RGB(179, 204, 102),  RGB(153, 153, 204) },
    { RGB(204, 204, 153),  RGB(153, 119, 204),  RGB(102, 153, 204),  RGB(153, 204, 204),  RGB(204, 153, 204) }
};

static void drawColorMatrix(BaseWidget_t * base){
    
    colorMatrix_t * colorMatrix = (colorMatrix_t *) base;
    

    int cellWidth = (base->pos.right - base->pos.left) / colorMatrix->width;
    int cellHeight = (base->pos.bottom - base->pos.top) / colorMatrix->height;

    int borderSize = 1; // border thickness
    COLORREF borderColor = RGB(100, 100, 100); // soft gray border

    for (int i = 0; i < colorMatrix->height; i++) {
        for (int j = 0; j < colorMatrix->width; j++) {
            AbsolutePos_t cellPos;
            cellPos.top = base->pos.top + i * cellHeight;
            cellPos.left = base->pos.left + j * cellWidth;
            cellPos.bottom = cellPos.top + cellHeight;
            cellPos.right = cellPos.left + cellWidth;

            COLORREF bgColor = colormap[i % 5][j % 5];

            UiUtils_DrawColoredRectangle(cellPos, bgColor, borderColor, borderSize);
        }
    }
    
}

static void onColorMatrixClick(BaseWidget_t * base, int x, int y){
    colorMatrix_t * colorMatrix = (colorMatrix_t *) base;
}

colorMatrix_t * initColorMatrix(CommonPos_t pos, int width, int height){
    colorMatrix_t *colorMatrix = (colorMatrix_t *)calloc(1, sizeof(colorMatrix_t));
    
    colorMatrix->baseWidget.initPos = pos;
    colorMatrix->height = height;
    colorMatrix->width = width;
    colorMatrix->baseWidget.drawHandler = &drawColorMatrix;
    colorMatrix->baseWidget.onClick = &onColorMatrixClick;

    return colorMatrix;
}
