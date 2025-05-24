#include "naricaMap.h"
#include "UiUtils.h"
#include <math.h>

static COLORREF TileTypeToColor(tileType_t tileType) {
    switch (tileType) {
        case TILE_TOWN:   return RGB(150, 100, 50);     // brown-ish
        case TILE_TOWN_CENTER:  return RGB(255, 255, 255);     // green-ish
        // Add other tile types and colors here
        default:          return RGB(100, 100, 100);    // fallback gray
    }
}


static void drawNarciaMap(BaseWidget_t * base){
    narciaMap_t * map = (narciaMap_t *) base;

    const int mapSize = map->mapSize;
    const int tileSize = map->tileSize;

    int left = base->pos.left;
    int top = base->pos.top;
    int right = base->pos.right;
    int bottom = base->pos.bottom;

    int widgetWidth = right - left;
    int widgetHeight = bottom - top;

    int centerX = left + widgetWidth / 2;
    int centerY = top + widgetHeight / 2;

    int middleTileTopLeftX = centerX - tileSize / 2;
    int middleTileTopLeftY = centerY - tileSize / 2;

    float tilesLeftF = ((float)(centerX - left) + tileSize / 2.0f) / tileSize;
    float tilesRightF = ((float)(right - centerX) + tileSize / 2.0f) / tileSize;
    float tilesUpF = ((float)(centerY - top) + tileSize / 2.0f) / tileSize;
    float tilesDownF = ((float)(bottom - centerY) + tileSize / 2.0f) / tileSize;

    int startX = map->middleX - (int)ceil(tilesLeftF) + 1;
    int endX = map->middleX + (int)ceil(tilesRightF);
    int startY = map->middleY - (int)ceil(tilesUpF) + 1;
    int endY = map->middleY + (int)ceil(tilesDownF);

    if (startX < 0) startX = 0;
    if (endX >= mapSize) endX = mapSize - 1;
    if (startY < 0) startY = 0;
    if (endY >= mapSize) endY = mapSize - 1;

    int borderSize = 1;
    COLORREF borderColor = RGB(80, 80, 80);

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            int tileLeft = middleTileTopLeftX + (x - map->middleX) * tileSize;
            int tileTop = middleTileTopLeftY + (y - map->middleY) * tileSize;
            int tileRight = tileLeft + tileSize;
            int tileBottom = tileTop + tileSize;

            if (tileRight <= left || tileLeft >= right || tileBottom <= top || tileTop >= bottom)
                continue;

            AbsolutePos_t tileRect;
            tileRect.left = tileLeft < left ? left : tileLeft;
            tileRect.top = tileTop < top ? top : tileTop;
            tileRect.right = tileRight > right ? right : tileRight;
            tileRect.bottom = tileBottom > bottom ? bottom : tileBottom;

            tileType_t tileType = map->map[y][x];

            COLORREF bgColor;
            if (tileType == TILE_EMPTY) {
                bgColor = ((x + y) % 2 == 0) ? RGB(140, 182, 58) : RGB(115, 157, 47);
            } else {
                bgColor = TileTypeToColor(tileType);
            }

            UiUtils_DrawColoredRectangle(tileRect, bgColor, borderColor, borderSize);
        }
    }
}

static void onClickNarciaMap(BaseWidget_t * base, int x, int y){
    narciaMap_t * narciaMap = (narciaMap_t *) base;
}

void PopulateMapWithTowns(narciaMap_t *map, Coordinate_t *coords, int coordCount) {
    for (int i = 0; i < coordCount; i++) {
        int cx = coords[i].x;
        int cy = coords[i].y;

        // Bounds check for center tile
        if (cx < 0 || cx >= map->mapSize || cy < 0 || cy >= map->mapSize)
            continue;

        // Set center tile to town_center
        map->map[cy][cx] = TILE_TOWN_CENTER;

        // Loop over neighbors (-1, 0, +1 in both x and y), including diagonals
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue; // skip center tile itself

                int nx = cx + dx;
                int ny = cy + dy;

                // Check bounds
                if (nx >= 0 && nx < map->mapSize && ny >= 0 && ny < map->mapSize) {
                    // Only set to TILE_TOWN if it's not already a town center
                    if (map->map[ny][nx] != TILE_TOWN_CENTER) {
                        map->map[ny][nx] = TILE_TOWN;
                    }
                }
            }
        }
    }
}

narciaMap_t * initNarciaMap(CommonPos_t pos){
    narciaMap_t *narciaMap = (narciaMap_t *)calloc(1, sizeof(narciaMap_t));
    
    narciaMap->baseWidget.initPos = pos;
    narciaMap->baseWidget.drawHandler = &drawNarciaMap;
    narciaMap->baseWidget.onClick = &onClickNarciaMap;
    narciaMap->middleX = 89;
    narciaMap->middleY = 89;
    narciaMap->tileSize = 20;
    narciaMap->mapSize = 180;

    PopulateMapWithTowns(narciaMap, narciaWarEraTowns, narciaWarEraTowns_count);

    return narciaMap;
}

