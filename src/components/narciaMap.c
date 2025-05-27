#include "UiUtils.h"
#include "naricaMap.h"
#include <math.h>
#include <stdio.h>

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

static void drawTown(AbsolutePos_t pos, mapTile_t mapTile, COLORREF color) {
	int borderWidth = 3;

	switch (mapTile.type) {
	case TILE_TOWN_TOP:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.right, pos.top}, color, borderWidth);
		break;

	case TILE_TOWN_BOTTOM:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.bottom, pos.left, pos.right, pos.bottom}, color, borderWidth);
		break;

	case TILE_TOWN_LEFT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.left, pos.bottom}, color, borderWidth);
		break;

	case TILE_TOWN_RIGHT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.right, pos.right, pos.bottom}, color, borderWidth);
		break;

	case TILE_TOWN_TOP_LEFT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.right, pos.top}, color, borderWidth);
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.left, pos.bottom}, color, borderWidth);
		break;

	case TILE_TOWN_TOP_RIGHT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.right, pos.top}, color, borderWidth);
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.right, pos.right, pos.bottom}, color, borderWidth);
		break;

	case TILE_TOWN_BOTTOM_LEFT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.bottom, pos.left, pos.right, pos.bottom}, color, borderWidth);
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.left, pos.bottom}, color, borderWidth);
		break;

	case TILE_TOWN_BOTTOM_RIGHT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.bottom, pos.left, pos.right, pos.bottom}, color, borderWidth);
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.right, pos.right, pos.bottom}, color, borderWidth);
		break;

	case TILE_TOWN_CENTER:
		// Optionally, draw a cross or leave it blank
		break;

	default:
		break;
	}
}

bool coordinateEqual(Coordinate_t c1, Coordinate_t c2) { return (c1.x == c2.x && c1.y == c2.y); }

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

	// draw background
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

			mapTile_t mapTile = map->map[y][x];

			if (mapTile.type != TILE_EMPTY) {
				continue;
			}

			COLORREF bgColor = RGB(115, 157, 47);

			if (coordinateEqual(map->selected1, (Coordinate_t){y, x}) || coordinateEqual(map->selected2, (Coordinate_t){y, x})) {
				bgColor = RGB(255, 255, 255);
			} else if (map->map[y][x].type == TILE_EMPTY && (x + y) % 2 == 0) {
				continue;
			}

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

			UiUtils_DrawColoredRectangle(tileRect, bgColor, borderColor, 0);
		}
	}

	for (int y = startY; y <= endY; y++) {
		for (int x = startX; x <= endX; x++) {

			mapTile_t mapTile = map->map[y][x];

			if (mapTile.type == TILE_EMPTY) {
				continue;
			}

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

			bool selected1Valid = (map->selected1.x >= 0 && map->selected1.y >= 0);
			bool selected2Valid = (map->selected2.x >= 0 && map->selected2.y >= 0);

			bool selected = false;

			if (selected1Valid && mapTile.townID == map->map[map->selected1.x][map->selected1.y].townID) {
				selected = true;
			}
			if (selected2Valid && mapTile.townID == map->map[map->selected2.x][map->selected2.y].townID) {
				selected = true;
			}

			if (selected) {
				drawTown(tileRect, mapTile, RGB(255, 255, 255));
			} else {
				drawTown(tileRect, mapTile, TileTypeToColor(mapTile));
			}
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
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hwnd, &mousePos);

	if (abs(activeDragStartX - mousePos.x) < 3 || 3 > abs(activeDragStartY - mousePos.y)) {

		BaseWidget_t base = activeNaricMap->baseWidget;
		narciaMap_t *narciaMap = activeNaricMap;

		// Setup
		int widgetLeft = base.pos.left;
		int widgetTop = base.pos.top;
		int widgetRight = base.pos.right;
		int widgetBottom = base.pos.bottom;

		int widgetWidth = widgetRight - widgetLeft;
		int widgetHeight = widgetBottom - widgetTop;

		int centerX = widgetLeft + widgetWidth / 2;
		int centerY = widgetTop + widgetHeight / 2;

		int tileSize = narciaMap->tileSize;

		// Get top-left corner of middle tile, same as drawing
		int middleTileTopLeftX = centerX - tileSize / 2;
		int middleTileTopLeftY = centerY - tileSize / 2;

		// Now compute the top-left of the full visible map area (drawing origin)
		int tilesLeft = (centerX - widgetLeft + tileSize / 2) / tileSize;
		int tilesUp = (centerY - widgetTop + tileSize / 2) / tileSize;

		int startX = narciaMap->middleX - tilesLeft + 1;
		int startY = narciaMap->middleY - tilesUp + 1;

		int drawOriginX = middleTileTopLeftX + (startX - narciaMap->middleX) * tileSize;
		int drawOriginY = middleTileTopLeftY + (startY - narciaMap->middleY) * tileSize;

		// Convert mouse (x, y) into tile indices
		int pixelOffsetX = activeDragStartX - drawOriginX;
		int pixelOffsetY = activeDragStartY - drawOriginY;

		if (pixelOffsetX < 0 || pixelOffsetY < 0) {
			return 0; // Clicked outside draw area
		}

		int tileOffsetX = pixelOffsetX / tileSize;
		int tileOffsetY = pixelOffsetY / tileSize;

		int tileX = startX + tileOffsetX;
		int tileY = startY + tileOffsetY;

		// Bounds check
		if (tileX >= 0 && tileX < narciaMap->mapSize && tileY >= 0 && tileY < narciaMap->mapSize) {

			Coordinate_t clicked = {tileY, tileX};
			Coordinate_t selected1 = narciaMap->selected1;
			Coordinate_t selected2 = narciaMap->selected2;

			if ((coordinateEqual(selected1, clicked)) || ((selected1.x >= 0 && selected1.y >= 0) && (narciaMap->map[tileY][tileX].townID == narciaMap->map[selected1.x][selected1.y].townID) &&(narciaMap->map[tileY][tileX].townID != 0))) {
				narciaMap->selected1 = (Coordinate_t){-1, -1};
			} else if ((coordinateEqual(selected2, clicked)) || ((selected2.x >= 0 && selected2.y) >= 0 && (narciaMap->map[tileY][tileX].townID == narciaMap->map[selected2.x][selected2.y].townID) &&(narciaMap->map[tileY][tileX].townID != 0))) {
				narciaMap->selected2 = (Coordinate_t){-1, -1};
			} else if (selected1.x == -1) {
				narciaMap->selected1 = clicked;
			} else if (selected2.x == -1) {
				narciaMap->selected2 = clicked;
			} else {
				narciaMap->selected1 = selected2;
				narciaMap->selected2 = clicked;
			}
		}
	}

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

bool insideofMap(narciaMap_t *map, int x, int y) {
	bool xCheck = false;
	bool yCheck = false;

	if (x < map->mapSize && x >= 0) {
		xCheck = true;
	}

	if (y < map->mapSize && y >= 0) {
		yCheck = true;
	}

	return xCheck && yCheck;
}

void PopulateMapWithTowns(narciaMap_t *map, Coordinate_t *coords, int coordCount, townType_t townType, int startID) {

	int id = startID;

	for (int i = 0; i < coordCount; i++) {

		int townX = coords[i].x;
		int townY = coords[i].y;

		if (insideofMap(map, townX - 1, townY - 1)) {
			map->map[townY - 1][townX - 1].type = TILE_TOWN_TOP_LEFT;
			map->map[townY - 1][townX - 1].townType = townType;
			map->map[townY - 1][townX - 1].townID = id;
		}

		if (insideofMap(map, townX, townY - 1)) {
			map->map[townY - 1][townX].type = TILE_TOWN_TOP;
			map->map[townY - 1][townX].townType = townType;
			map->map[townY - 1][townX].townID = id;
		}

		if (insideofMap(map, townX + 1, townY - 1)) {
			map->map[townY - 1][townX + 1].type = TILE_TOWN_TOP_RIGHT;
			map->map[townY - 1][townX + 1].townType = townType;
			map->map[townY - 1][townX + 1].townID = id;
		}

		if (insideofMap(map, townX - 1, townY)) {
			map->map[townY][townX - 1].type = TILE_TOWN_LEFT;
			map->map[townY][townX - 1].townType = townType;
			map->map[townY][townX - 1].townID = id;
		}

		if (insideofMap(map, townX, townY)) {
			map->map[townY][townX].type = TILE_TOWN_CENTER;
			map->map[townY][townX].townType = townType;
			map->map[townY][townX].townID = id;
		}

		if (insideofMap(map, townX + 1, townY)) {
			map->map[townY][townX + 1].type = TILE_TOWN_RIGHT;
			map->map[townY][townX + 1].townType = townType;
			map->map[townY][townX + 1].townID = id;
		}

		if (insideofMap(map, townX - 1, townY + 1)) {
			map->map[townY + 1][townX - 1].type = TILE_TOWN_BOTTOM_LEFT;
			map->map[townY + 1][townX - 1].townType = townType;
			map->map[townY + 1][townX - 1].townID = id;
		}

		if (insideofMap(map, townX, townY + 1)) {
			map->map[townY + 1][townX].type = TILE_TOWN_BOTTOM;
			map->map[townY + 1][townX].townType = townType;
			map->map[townY + 1][townX].townID = id;
		}

		if (insideofMap(map, townX + 1, townY + 1)) {
			map->map[townY + 1][townX + 1].type = TILE_TOWN_BOTTOM_RIGHT;
			map->map[townY + 1][townX + 1].townType = townType;
			map->map[townY + 1][townX + 1].townID = id;
		}

		id++;
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

	narciaMap->selected1 = (Coordinate_t){-1, -1};
	narciaMap->selected2 = (Coordinate_t){-1, -1};

	PopulateMapWithTowns(narciaMap, narciaWarEraTowns, narciaWarEraTowns_count, TOWN_TYPE_SMALL, 1);
	PopulateMapWithTowns(narciaMap, narciaWarEraLargeTowns, narciaWarEraLargeTowns_count, TOWN_TYPE_LARGE, narciaWarEraTowns_count + 1);
	PopulateMapWithTowns(narciaMap, narciaWarEraImperalCastle, narciaWarEraImperalCastle_count, TOWN_TYPE_IMPERIAL_CASTLE, narciaWarEraLargeTowns_count + narciaWarEraTowns_count + 1);

	if (!callbackInitialized) {
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEMOVE, &mouseMoveNarciaMap);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_LBUTTONUP, &buttonUpCallbackNaricaMap);
		WmParamHanderTable_Insert(currentWindowState.handlerTable, WM_MOUSEWHEEL, &handleMouseWheelNarcia);
	}

	return narciaMap;
}

void narciaMapAddPath(narciaMap_t *map, path_t *path) { DynamicArray_Add(map->paths, path); }