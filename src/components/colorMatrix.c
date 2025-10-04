#include "colorMatrix.h"

#include "UiUtils.h"

COLORREF Colormap[COLOR_MATRIX_ROWS][COLOR_MATRIX_COLS] = {
    {RGB(166, 94, 94), RGB(146, 114, 146), RGB(94, 114, 166), RGB(166, 140, 94), RGB(94, 146, 166)},
    {RGB(130, 80, 100), RGB(120, 100, 150), RGB(100, 130, 160), RGB(180, 130, 90),
     RGB(140, 160, 190)},
    {RGB(190, 120, 120), RGB(160, 120, 190), RGB(100, 160, 190), RGB(200, 190, 100),
     RGB(130, 140, 160)},
    {RGB(180, 160, 160), RGB(140, 120, 200), RGB(110, 140, 170), RGB(180, 190, 110),
     RGB(160, 160, 200)},
    {RGB(210, 180, 140), RGB(170, 130, 200), RGB(120, 150, 200), RGB(160, 200, 210),
     RGB(200, 160, 200)}};

static void drawColorMatrix(BaseWidget_t* base) {
    colorMatrix_t* colorMatrix = (colorMatrix_t*)base;

    int cellWidth = (base->pos.right - base->pos.left) / colorMatrix->width;
    int cellHeight = (base->pos.bottom - base->pos.top) / colorMatrix->height;

    int borderSize = 2;                         // border thickness
    COLORREF borderColor = RGB(100, 100, 100);  // soft gray border

    for (int i = 0; i < colorMatrix->height; i++) {
        for (int j = 0; j < colorMatrix->width; j++) {
            AbsolutePos_t cellPos;
            cellPos.top = base->pos.top + i * cellHeight;
            cellPos.left = base->pos.left + j * cellWidth;
            cellPos.bottom = cellPos.top + cellHeight;
            cellPos.right = cellPos.left + cellWidth;

            COLORREF bgColor = Colormap[i % COLOR_MATRIX_ROWS][j % COLOR_MATRIX_COLS];

            if (colorMatrix->activeColumn == j && colorMatrix->activeRow == i) {
                UiUtils_DrawColoredRectangle(cellPos, bgColor, RGB(0, 122, 204), borderSize);
            } else {
                UiUtils_DrawColoredRectangle(cellPos, bgColor, borderColor, borderSize);
            }
        }
    }
}

static void onColorMatrixClick(BaseWidget_t* base, int x, int y, ClickType_t clickType) {
    colorMatrix_t* colorMatrix = (colorMatrix_t*)base;
    (void)x;
    (void)y;
    (void)clickType;

    if (clickType == CLICK_TYPE_LUP) {
        if (colorMatrix && base) {
            int relativeX = x - base->pos.left;
            int relativeY = y - base->pos.top;
            int cellWidth = (base->pos.right - base->pos.left) / colorMatrix->width;
            int cellHeight = (base->pos.bottom - base->pos.top) / colorMatrix->height;

            if (relativeX > 0 || relativeY > 0) {
                int col = relativeX / cellWidth;
                int row = relativeY / cellHeight;

                // max size for now
                if (col < COLOR_MATRIX_COLS && row < COLOR_MATRIX_ROWS) {
                    if (colorMatrix->activeRow == row && colorMatrix->activeColumn == col) {
                        colorMatrix->activeRow = -1;
                        colorMatrix->activeColumn = -1;
                    } else {
                        colorMatrix->activeRow = row;
                        colorMatrix->activeColumn = col;
                    }
                }
            }
        }
    }
}

COLORREF colorMatrixGetActive(colorMatrix_t* colorMatrix) {
    if (colorMatrix->activeRow >= 0 && colorMatrix->activeColumn >= 0) {
        return Colormap[colorMatrix->activeRow][colorMatrix->activeColumn];
    }
    // fallback color
    return RGB(255, 255, 255);
}

bool colorMatrixHasActive(colorMatrix_t* colorMatrix) {
    if (colorMatrix->activeRow == -1 || colorMatrix->activeColumn == -1) {
        return false;
    }
    return true;
}

colorMatrix_t* initColorMatrix(CommonPos_t pos, int width, int height) {
    colorMatrix_t* colorMatrix = (colorMatrix_t*)calloc(1, sizeof(colorMatrix_t));

    colorMatrix->baseWidget.initPos = pos;
    colorMatrix->height = height;
    colorMatrix->width = width;
    colorMatrix->activeRow = -1;
    colorMatrix->activeColumn = -1;
    colorMatrix->baseWidget.drawHandler = &drawColorMatrix;
    colorMatrix->baseWidget.onClick = &onColorMatrixClick;

    return colorMatrix;
}
