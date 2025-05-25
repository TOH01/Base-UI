#include "UiUtils.h"
#include "naricaMap.h"
#include <math.h>

narciaMap_t *focusedNarciaMap = NULL;

narciaMap_t *activeNaricMap = NULL;
int activeDragStartX = 0;
int activeDragStartY = 0;
int dragStartMiddleX = 0;
int dragStartMiddleY = 0;

bool callbackInitialized = false;

static COLORREF TileTypeToColor(mapTile_t mapTile) {
	switch (mapTile.townType) {
	case TOWN_TYPE_LARGE:
		return RGB(123, 77, 247);
	case TOWN_TYPE_IMPERIAL_CASTLE:
		return RGB(181, 20, 247);
	case TOWN_TYPE_SMALL:
		return RGB(148, 89, 247);
	default:
		return RGB(100, 100, 100);
	}
}

static void drawTown(AbsolutePos_t pos){

}

static void drawNarciaMap(BaseWidget_t *base) {
	narciaMap_t *map = (narciaMap_t *)base;

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
	float tilesRightF = tilesLeftF;
	float tilesUpF = ((float)(centerY - top) + tileSize / 2.0f) / tileSize;
	float tilesDownF = tilesUpF;

	int startX = map->middleX - (int)ceil(tilesLeftF) + 1;
	int endX = map->middleX + (int)ceil(tilesRightF);
	int startY = map->middleY - (int)ceil(tilesUpF) + 1;
	int endY = map->middleY + (int)ceil(tilesDownF);

	if (startX < 0)
		startX = 0;
	if (endX >= mapSize)
		endX = mapSize - 1;
	if (startY < 0)
		startY = 0;
	if (endY >= mapSize)
		endY = mapSize - 1;

	int borderSize = 1;
	COLORREF borderColor = RGB(80, 80, 80);

    //draw background
    int tileDrawLeft = middleTileTopLeftX + (startX - map->middleX) * tileSize;
    int tileDrawTop = middleTileTopLeftY + (startY - map->middleY) * tileSize;
    int tileDrawRight = middleTileTopLeftX + (endX - map->middleX + 1) * tileSize;
    int tileDrawBottom = middleTileTopLeftY + (endY - map->middleY + 1) * tileSize;

    AbsolutePos_t backgroundPos;
    backgroundPos.left = max(tileDrawLeft, left);
    backgroundPos.top = max(tileDrawTop, top);
    backgroundPos.right = min(tileDrawRight, right);
    backgroundPos.bottom = min(tileDrawBottom, bottom);

    UiUtils_DrawColoredRectangle(backgroundPos, RGB(140, 182, 58), borderColor, 0); // no border


    int middleXMap = map->middleX;
    int middleYMap = map->middleY;

	for (int y = startY; y <= endY; y++) {
		for (int x = startX; x <= endX; x++) {
			
            if(map->map[y][x].type == TILE_EMPTY && (x + y) % 2 == 0){continue;}
            
            int tileLeft = middleTileTopLeftX + (x - middleXMap) * tileSize;
			int tileTop = middleTileTopLeftY + (y - middleYMap) * tileSize;
			int tileRight = tileLeft + tileSize;
			int tileBottom = tileTop + tileSize;

			if (tileRight <= left || tileLeft >= right || tileBottom <= top || tileTop >= bottom)
				continue;

			AbsolutePos_t tileRect;
			tileRect.left = tileLeft < left ? left : tileLeft;
			tileRect.top = tileTop < top ? top : tileTop;
			tileRect.right = tileRight > right ? right : tileRight;
			tileRect.bottom = tileBottom > bottom ? bottom : tileBottom;

			mapTile_t mapTile = map->map[y][x];

			COLORREF bgColor;
			if (mapTile.type == TILE_EMPTY) {
				bgColor = ((x + y) % 2 == 0) ? RGB(140, 182, 58) : RGB(115, 157, 47);
			} else {
				bgColor = TileTypeToColor(mapTile);
			}

			UiUtils_DrawColoredRectangle(tileRect, bgColor, borderColor, 0);
		}
	}
}

static void onClickNarciaMap(BaseWidget_t *base, int x, int y) {
	narciaMap_t *narciaMap = (narciaMap_t *)base;

	if (focusedNarciaMap == NULL) {
		focusedNarciaMap = narciaMap;
	}

	if (activeNaricMap == NULL) {
		activeNaricMap = narciaMap;
		activeDragStartX = x;
		activeDragStartY = y;
		dragStartMiddleX = narciaMap->middleX;
		dragStartMiddleY = narciaMap->middleY;
	}
}

LRESULT buttonUpCallbackNaricaMap(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (activeNaricMap) {
		activeNaricMap = NULL;
	}
}

LRESULT handleMouseWheelNarcia(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (focusedNarciaMap) {

		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(hwnd, &mousePos);

		if (UiUtils_CoordinateIsInsideOf(mousePos.x, mousePos.y, focusedNarciaMap->baseWidget.pos)) {

			int delta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (delta > 0) {
				if (focusedNarciaMap->tileSize < focusedNarciaMap->maxTileSize) {
					focusedNarciaMap->tileSize += focusedNarciaMap->zoomStep;
					InvalidateRect(hwnd, NULL, FALSE);
				}
			} else {

				if (focusedNarciaMap->tileSize > focusedNarciaMap->minTileSize) {
					focusedNarciaMap->tileSize -= focusedNarciaMap->zoomStep;
					InvalidateRect(hwnd, NULL, FALSE);
				}
			}
		}
	}
}

void goToTile(narciaMap_t *narciaMap, int x, int y) {
	narciaMap->middleX = x;
	narciaMap->middleY = y;
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

LRESULT mouseMoveNarciaMap(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (activeNaricMap == NULL)
		return 0;

	int mouseX = GET_X_PARAM(lParam);
	int mouseY = GET_Y_PARAM(lParam);

	// Check if mouse is still within widget bounds
	AbsolutePos_t pos = activeNaricMap->baseWidget.pos;
	if (mouseX < pos.left || mouseX > pos.right || mouseY < pos.top || mouseY > pos.bottom) {
		activeNaricMap = NULL; // Cancel drag
		return 0;
	}

	// Calculate how much the mouse moved
	int dx = mouseX - activeDragStartX;
	int dy = mouseY - activeDragStartY;

	// Translate pixel offset to tile offset
	int tileOffsetX = dx / activeNaricMap->tileSize;
	int tileOffsetY = dy / activeNaricMap->tileSize;

	int newMiddleX = dragStartMiddleX - tileOffsetX;
	int newMiddleY = dragStartMiddleY - tileOffsetY;

	// Clamp to map bounds
	if (newMiddleX < 0)
		newMiddleX = 0;
	if (newMiddleY < 0)
		newMiddleY = 0;
	if (newMiddleX >= activeNaricMap->mapSize)
		newMiddleX = activeNaricMap->mapSize - 1;
	if (newMiddleY >= activeNaricMap->mapSize)
		newMiddleY = activeNaricMap->mapSize - 1;

	goToTile(activeNaricMap, newMiddleX, newMiddleY);

	return 0;
}

void PopulateMapWithTowns(narciaMap_t *map, Coordinate_t *coords, int coordCount, townType_t townType) {
	for (int i = 0; i < coordCount; i++) {
		int cx = coords[i].x;
		int cy = coords[i].y;

		// Bounds check for center tile
		if (cx < 0 || cx >= map->mapSize || cy < 0 || cy >= map->mapSize)
			continue;

		// Set center tile to town_center
		map->map[cy][cx].type = TILE_TOWN_CENTER;
		map->map[cy][cx].townType = townType;

		// Loop over neighbors (-1, 0, +1 in both x and y), including diagonals
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0)
					continue; // skip center tile itself

				int nx = cx + dx;
				int ny = cy + dy;

				// Check bounds
				if (nx >= 0 && nx < map->mapSize && ny >= 0 && ny < map->mapSize) {
					// Only set to TILE_TOWN if it's not already a town center
					if (map->map[ny][nx].type != TILE_TOWN_CENTER) {
						map->map[ny][nx].type = TILE_TOWN;
						map->map[ny][nx].townType = townType;
					}
				}
			}
		}
	}
}

narciaMap_t *initNarciaMap(CommonPos_t pos) {
	narciaMap_t *narciaMap = (narciaMap_t *)calloc(1, sizeof(narciaMap_t));

	narciaMap->baseWidget.initPos = pos;
	narciaMap->baseWidget.drawHandler = &drawNarciaMap;
	narciaMap->baseWidget.onClick = &onClickNarciaMap;
	narciaMap->middleX = 89;
	narciaMap->middleY = 89;
	narciaMap->tileSize = 20;
	narciaMap->mapSize = 180;
	narciaMap->zoomStep = 1;
	narciaMap->maxTileSize = 40;
	narciaMap->minTileSize = 10;

	PopulateMapWithTowns(narciaMap, narciaWarEraTowns, narciaWarEraTowns_count, TOWN_TYPE_SMALL);
	PopulateMapWithTowns(narciaMap, narciaWarEraLargeTowns, narciaWarEraLargeTowns_count, TOWN_TYPE_LARGE);
	PopulateMapWithTowns(narciaMap, narciaWarEraImperalCastle, narciaWarEraImperalCastle_count, TOWN_TYPE_IMPERIAL_CASTLE);

	if (!callbackInitialized) {
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEMOVE, &mouseMoveNarciaMap);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_LBUTTONUP, &buttonUpCallbackNaricaMap);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEWHEEL, &handleMouseWheelNarcia);
	}

	return narciaMap;
}

void narciaMapAddPath(narciaMap_t *map, path_t *path) { DynamicArray_Add(map->paths, path); }