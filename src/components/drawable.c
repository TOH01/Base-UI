#include "drawable.h"

#include <stdio.h>

#include "UiUtils.h"

Drawable_t* drawable_initRectangle(CommonPos_t pos, rectangleTheme_t* theme) {
    Drawable_t* drawable = (Drawable_t*)calloc(1, sizeof(Drawable_t));
    drawable->type = DRAWABLE_RECTANGLE;

    drawable->rectangle.theme = theme;

    drawable->initPos = pos;

    return drawable;
}

Drawable_t* drawable_initLabel(CommonPos_t pos, const char* text, labelTheme_t* theme) {
    Drawable_t* drawable = (Drawable_t*)calloc(1, sizeof(Drawable_t));
    drawable->type = DRAWABLE_LABEL;

    drawable->label.theme = theme;

    drawable->label.text = text;

    drawable->initPos = pos;

    return drawable;
}

Drawable_t* drawable_initLine(CommonPos_t pos, lineTheme_t* theme) {
    Drawable_t* drawable = (Drawable_t*)calloc(1, sizeof(Drawable_t));
    drawable->type = DRAWABLE_LINE;
    drawable->line.theme = theme;

    drawable->initPos = pos;

    return drawable;
}

Drawable_t* drawable_initImg(CommonPos_t pos, int ID) {
    Drawable_t* drawable = (Drawable_t*)calloc(1, sizeof(Drawable_t));
    drawable->type = DRAWABLE_IMG;
    drawable->initPos = pos;
    drawable->img.iconID = ID;

    return drawable;
}

Drawable_t* drawable_init3SliceImgRectange(CommonPos_t pos, int ID_LEFT, int ID_CENTER,
                                           int ID_RIGHT) {
    Drawable_t* drawable = (Drawable_t*)calloc(1, sizeof(Drawable_t));
    drawable->type = DRAWABLE_3_SLICE_IMG_RECT;
    drawable->initPos = pos;

    drawable->slice3img.iconID[0] = ID_LEFT;
    drawable->slice3img.iconID[1] = ID_CENTER;
    drawable->slice3img.iconID[2] = ID_RIGHT;

    return drawable;
}

Drawable_t* drawable_init9SliceImgRectangle(CommonPos_t pos, int ID_TOP_LEFT, int ID_TOP_CENTER,
                                            int ID_TOP_RIGHT, int ID_MID_LEFT, int ID_CENTER,
                                            int ID_MID_RIGHT, int ID_BOTTOM_LEFT,
                                            int ID_BOTTOM_CENTER, int ID_BOTTOM_RIGHT,
                                            int slizeSize) {
    Drawable_t* drawable = (Drawable_t*)calloc(1, sizeof(Drawable_t));
    drawable->type = DRAWABLE_9_SLICE_IMG_RECT;
    drawable->initPos = pos;

    drawable->slice9img.iconID[0] = ID_TOP_LEFT;
    drawable->slice9img.iconID[1] = ID_TOP_CENTER;
    drawable->slice9img.iconID[2] = ID_TOP_RIGHT;
    drawable->slice9img.iconID[3] = ID_MID_LEFT;
    drawable->slice9img.iconID[4] = ID_CENTER;
    drawable->slice9img.iconID[5] = ID_MID_RIGHT;
    drawable->slice9img.iconID[6] = ID_BOTTOM_LEFT;
    drawable->slice9img.iconID[7] = ID_BOTTOM_CENTER;
    drawable->slice9img.iconID[8] = ID_BOTTOM_RIGHT;
    drawable->slice9img.sliceSize = slizeSize;

    return drawable;
}

void drawable_draw(Drawable_t* drawable) {
    if (drawable->hidden) {
        return;
    }

    switch (drawable->type) {
        case DRAWABLE_LINE:

            lineTheme_t* theme = drawable->line.theme;

            UiUtils_DrawLineRelativeTheme(drawable->pos, theme->color.fill, theme->width);

            break;
        case DRAWABLE_RECTANGLE:

            rectangleTheme_t* themeRectangle = drawable->rectangle.theme;

            UiUtils_DrawColoredRectangle(drawable->pos, themeRectangle->color.fill,
                                         themeRectangle->color.border, themeRectangle->borderWidth);
            break;
        case DRAWABLE_IMG:
            if (!drawable->img.iconLoaded) {
                drawable->img.icon = (HICON)LoadImage(
                    currentWindowState.hInstance, MAKEINTRESOURCE(drawable->img.iconID), IMAGE_ICON,
                    drawable->pos.right - drawable->pos.left,
                    drawable->pos.bottom - drawable->pos.top, LR_DEFAULTCOLOR);
                drawable->img.iconLoaded = true;
            }

            DrawIconEx(currentWindowState.memHDC, drawable->pos.left, drawable->pos.top,
                       drawable->img.icon, drawable->pos.right - drawable->pos.left,
                       drawable->pos.bottom - drawable->pos.top, 0, NULL, DI_NORMAL);
            break;
        case DRAWABLE_3_SLICE_IMG_RECT:
            if (!drawable->slice3img.iconsLoades) {
                drawable->slice3img.icon[0] = (HICON)LoadImage(
                    currentWindowState.hInstance, MAKEINTRESOURCE(drawable->slice3img.iconID[0]),
                    IMAGE_ICON, drawable->pos.right - drawable->pos.left,
                    drawable->pos.bottom - drawable->pos.top, LR_DEFAULTCOLOR);
                drawable->slice3img.icon[1] = (HICON)LoadImage(
                    currentWindowState.hInstance, MAKEINTRESOURCE(drawable->slice3img.iconID[1]),
                    IMAGE_ICON, drawable->pos.right - drawable->pos.left,
                    drawable->pos.bottom - drawable->pos.top, LR_DEFAULTCOLOR);
                drawable->slice3img.icon[2] = (HICON)LoadImage(
                    currentWindowState.hInstance, MAKEINTRESOURCE(drawable->slice3img.iconID[2]),
                    IMAGE_ICON, drawable->pos.right - drawable->pos.left,
                    drawable->pos.bottom - drawable->pos.top, LR_DEFAULTCOLOR);
                drawable->slice3img.iconsLoades = true;
            }

            UiUtils_draw3SliceHelper(drawable->pos, drawable->slice3img.icon[0],
                                     drawable->slice3img.icon[1], drawable->slice3img.icon[2]);
            break;
        case DRAWABLE_9_SLICE_IMG_RECT:
            if (!drawable->slice9img.iconsLoades) {
                int width = drawable->pos.right - drawable->pos.left;
                int height = drawable->pos.bottom - drawable->pos.top;

                for (int i = 0; i < 9; ++i) {
                    drawable->slice9img.icon[i] =
                        (HICON)LoadImage(currentWindowState.hInstance,
                                         MAKEINTRESOURCE(drawable->slice9img.iconID[i]), IMAGE_ICON,
                                         width, height, LR_DEFAULTCOLOR);
                }

                drawable->slice9img.iconsLoades = true;
            }

            draw9SliceHelper(drawable->pos, drawable->slice9img.icon[0],
                             drawable->slice9img.icon[1], drawable->slice9img.icon[2],
                             drawable->slice9img.icon[3], drawable->slice9img.icon[4],
                             drawable->slice9img.icon[5], drawable->slice9img.icon[6],
                             drawable->slice9img.icon[7], drawable->slice9img.icon[8],
                             drawable->slice9img.sliceSize, drawable->slice9img.sliceSize);
            break;
        case DRAWABLE_LABEL:

            labelTheme_t* themeLabel = drawable->label.theme;

            if (UiUtils_TextFitsBoxTheme(drawable->label.text, drawable->pos,
                                         themeLabel->text.font)) {
                UiUtils_DrawTextTheme(drawable->pos, drawable->label.text,
                                      themeLabel->text.formatFlags, themeLabel->text.font,
                                      themeLabel->text.color);
            }
#ifdef DEBUG
            else {
                printf("Drawable text too large for label");
            }
#endif
            break;

        default:
            break;
    }
}

void addDrawable(DynamicArray_t* array, Drawable_t* drawable) {
    if (!array || !drawable) {
        return;
    }

    DynamicArray_Add(array, drawable);
}

void drawable_drawAll(DynamicArray_t* array) {
    if (!array) {
        return;
    }

    Drawable_t* drawable = NULL;

    for (int i = 0; i < array->size; i++) {
        drawable = (Drawable_t*)DynamicArray_get(array, i);

        drawable_draw(drawable);
    }
}

void drawable_updatePosToContainerList(DynamicArray_t* array) {
    if (!array) {
        return;
    }

    Drawable_t* drawable = NULL;

    for (int i = 0; i < array->size; i++) {
        drawable = (Drawable_t*)DynamicArray_get(array, i);

        if (drawable->posType == POS_TYPE_REL) {
            drawable->pos = getPosToContainer(drawable->parentPos, drawable->initPos);
        } else if (drawable->posType == POS_TYPE_ABS) {
            drawable->pos = getPosToContainerAbsolute(drawable->parentPos, drawable->initPosAbs);
        }
    }
}
