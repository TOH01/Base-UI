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

POINT TileToScreenCenter(narciaMap_t *map, Coordinate_t tile) {
	BaseWidget_t base = map->baseWidget;

	int widgetCenterX = (base.pos.left + base.pos.right) / 2;
	int widgetCenterY = (base.pos.top + base.pos.bottom) / 2;

	int dx = tile.x - map->middleX;
	int dy = tile.y - map->middleY;

	POINT p;
	p.x = widgetCenterX + dx * map->tileSize;
	p.y = widgetCenterY + dy * map->tileSize;
	return p;
}

bool screenPosToNarciaPos(narciaMap_t *map, int screenX, int screenY, Coordinate_t *outTile) {
	BaseWidget_t base = map->baseWidget;

	int widgetLeft = base.pos.left;
	int widgetTop = base.pos.top;
	int widgetRight = base.pos.right;
	int widgetBottom = base.pos.bottom;

	int centerX = (widgetLeft + widgetRight) / 2;
	int centerY = (widgetTop + widgetBottom) / 2;

	int tileSize = map->tileSize;

	int middleTileTopLeftX = centerX - tileSize / 2;
	int middleTileTopLeftY = centerY - tileSize / 2;

	int tilesLeft = (centerX - widgetLeft + tileSize / 2) / tileSize;
	int tilesUp = (centerY - widgetTop + tileSize / 2) / tileSize;

	int startX = map->middleX - tilesLeft + 1;
	int startY = map->middleY - tilesUp + 1;

	int drawOriginX = middleTileTopLeftX + (startX - map->middleX) * tileSize;
	int drawOriginY = middleTileTopLeftY + (startY - map->middleY) * tileSize;

	int offsetX = screenX - drawOriginX;
	int offsetY = screenY - drawOriginY;

	if (offsetX < 0 || offsetY < 0)
		return false;

	int tileX = startX + offsetX / tileSize;
	int tileY = startY + offsetY / tileSize;

	if (tileX < 0 || tileX >= map->mapSize || tileY < 0 || tileY >= map->mapSize)
		return false;

	*outTile = (Coordinate_t){tileX, tileY};
	return true;
}

AbsolutePos_t TileToScreenRect(narciaMap_t *map, int tileX, int tileY) {

	BaseWidget_t base = map->baseWidget;

	int left = base.pos.left;
	int top = base.pos.top;
	int right = base.pos.right;
	int bottom = base.pos.bottom;

	int widgetWidth = right - left;
	int widgetHeight = bottom - top;

	int centerX = left + widgetWidth / 2;
	int centerY = top + widgetHeight / 2;

	int tileSize = map->tileSize;

	int middleTileTopLeftX = centerX - tileSize / 2;
	int middleTileTopLeftY = centerY - tileSize / 2;

	int tileLeft = middleTileTopLeftX + (tileX - map->middleX) * tileSize;
	int tileTop = middleTileTopLeftY + (tileY - map->middleY) * tileSize;

	return (AbsolutePos_t){.left = tileLeft, .top = tileTop, .right = tileLeft + tileSize, .bottom = tileTop + tileSize};
}

static void drawPathOnMap(narciaMap_t *map, COLORREF color, Coordinate_t *path, int pathLength) {
	if (pathLength < 2)
		return;

	for (int i = 0; i < pathLength - 1; i++) {
		Coordinate_t from = path[i];
		Coordinate_t to = path[i + 1];

		POINT fromPoint = TileToScreenCenter(map, from);
		POINT toPoint = TileToScreenCenter(map, to);

		UiUtils_DrawLineRelative((AbsolutePos_t){
			.left = fromPoint.x,
			.top = fromPoint.y,
			.right = toPoint.x,
			.bottom = toPoint.y
		}, color, 4);
	}
}

Coordinate_t getCenterOfTownTile(narciaMap_t *map, Coordinate_t townTile) {

	Coordinate_t center = townTile;

	switch (map->map[townTile.y][townTile.x].type) {
	case TILE_TOWN_TOP:
		center.y += 1;
		break;
	case TILE_TOWN_BOTTOM:
		center.y -= 1;
		break;
	case TILE_TOWN_LEFT:
		center.x += 1;
		break;
	case TILE_TOWN_RIGHT:
		center.x -= 1;
		break;
	case TILE_TOWN_TOP_LEFT:
		center.x += 1;
		center.y += 1;
		break;
	case TILE_TOWN_TOP_RIGHT:
		center.x -= 1;
		center.y += 1;
		break;
	case TILE_TOWN_BOTTOM_LEFT:
		center.x += 1;
		center.y -= 1;
		break;
	case TILE_TOWN_BOTTOM_RIGHT:
		center.x -= 1;
		center.y -= 1;
		break;
	case TILE_TOWN_CENTER:
	default:
		break;
	}

	return center;
}

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

bool adjacentToTown(Coordinate_t townCenter, Coordinate_t tile) { return (abs(townCenter.x - tile.x) <= 2) && (abs(townCenter.y - tile.y) <= 2); }

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
			if (mapTile.type != TILE_EMPTY)
				continue;

			COLORREF bgColor = RGB(115, 157, 47);
			if (coordinateEqual(map->selected1, (Coordinate_t){x, y}) || coordinateEqual(map->selected2, (Coordinate_t){x, y})) {
				bgColor = RGB(255, 255, 255);
			} else if ((x + y) % 2 == 0) {
				continue;
			}

			AbsolutePos_t rect = TileToScreenRect(map, x, y);
			if (rect.right <= left || rect.left >= right || rect.bottom <= top || rect.top >= bottom)
				continue;

			rect.left = max(rect.left, left);
			rect.top = max(rect.top, top);
			rect.right = min(rect.right, right);
			rect.bottom = min(rect.bottom, bottom);

			UiUtils_DrawColoredRectangle(rect, bgColor, borderColor, 0);
		}
	}

	for (int y = startY; y <= endY; y++) {
		for (int x = startX; x <= endX; x++) {
			mapTile_t mapTile = map->map[y][x];
			if (mapTile.type == TILE_EMPTY)
				continue;

			AbsolutePos_t rect = TileToScreenRect(map, x, y);
			if (rect.right <= left || rect.left >= right || rect.bottom <= top || rect.top >= bottom)
				continue;

			rect.left = max(rect.left, left);
			rect.top = max(rect.top, top);
			rect.right = min(rect.right, right);
			rect.bottom = min(rect.bottom, bottom);

			bool selected = false;
			bool selected1Valid = (map->selected1.x >= 0 && map->selected1.y >= 0);
			bool selected2Valid = (map->selected2.x >= 0 && map->selected2.y >= 0);

			if (selected1Valid && mapTile.townID == map->map[map->selected1.y][map->selected1.x].townID)
				selected = true;
			if (selected2Valid && mapTile.townID == map->map[map->selected2.y][map->selected2.x].townID)
				selected = true;

			Coordinate_t townCenter = getCenterOfTownTile(map, (Coordinate_t){x, y});
			bool disabled = !map->map[townCenter.y][townCenter.x].active;

			if (disabled) {
				drawTown(rect, mapTile, RGB(184, 184, 184));
			} else if (selected) {
				drawTown(rect, mapTile, RGB(255, 255, 255));
			} else {
				drawTown(rect, mapTile, TileTypeToColor(mapTile));
			}
		}
	}

	if (map->paths) {
		int pathCount = map->paths->size;
		for (int i = 0; i < pathCount; i++) {
			path_t *path = (path_t *)DynamicArray_get(map->paths, i);
			if (!path || !path->tiles || path->tileCount < 2)
				continue;

			drawPathOnMap(map, path->color, path->tiles, path->tileCount);
		}
	}
}

void RbuttonDownCallbackNaricaMap(int x, int y, narciaMap_t *map) {
	Coordinate_t tile;
	if (screenPosToNarciaPos(map, x, y, &tile)) {
		mapTile_t clickedTile = map->map[tile.y][tile.x];
		if (clickedTile.townID != 0) {
			Coordinate_t center = getCenterOfTownTile(map, tile);
			map->map[center.y][center.x].active = !map->map[center.y][center.x].active;
		}
	}
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

static void onClickNarciaMap(BaseWidget_t *base, int x, int y, ClickType_t clickType) {
	narciaMap_t *narciaMap = (narciaMap_t *)base;

	switch (clickType) {
	case CLICK_TYPE_LDOWN:
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
		break;
	case CLICK_TYPE_RDOWN:
		RbuttonDownCallbackNaricaMap(x, y, narciaMap);
		break;
	default:
		break;
	}
}

bool isAdjacent(Coordinate_t c1, Coordinate_t c2) {
	if (coordinateEqual(c1, c2)) {
		return false;
	}

	return (abs(c1.x - c2.x) <= 1) && (abs(c1.y - c2.y) <= 1);
}

LRESULT buttonUpCallbackNaricaMap(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hwnd, &mousePos);

	if (abs(activeDragStartX - mousePos.x) < 3 && abs(activeDragStartY - mousePos.y) < 3) {
		narciaMap_t *map = activeNaricMap;
		Coordinate_t clicked;

		if (screenPosToNarciaPos(map, activeDragStartX, activeDragStartY, &clicked)) {
			mapTile_t tile = map->map[clicked.y][clicked.x];
			Coordinate_t townCenter = getCenterOfTownTile(map, clicked);
			if (tile.townID != 0 && map->map[townCenter.y][townCenter.x].active) {
				Coordinate_t selected1 = map->selected1;
				Coordinate_t selected2 = map->selected2;

				bool isSameTownAs1 = (selected1.x >= 0 && map->map[clicked.y][clicked.x].townID == map->map[selected1.y][selected1.x].townID);
				bool isSameTownAs2 = (selected2.x >= 0 && map->map[clicked.y][clicked.x].townID == map->map[selected2.y][selected2.x].townID);

				if (coordinateEqual(selected1, clicked) || isSameTownAs1) {
					map->selected1 = (Coordinate_t){-1, -1};
				} else if (coordinateEqual(selected2, clicked) || isSameTownAs2) {
					map->selected2 = (Coordinate_t){-1, -1};
				} else if (selected1.x == -1) {
					map->selected1 = clicked;
				} else if (selected2.x == -1) {
					map->selected2 = clicked;
				} else {
					map->selected1 = selected2;
					map->selected2 = clicked;
				}
			}
		}
	}

	activeNaricMap = NULL;
	return 0;
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

		int townX = coords[i].y;
		int townY = coords[i].x;

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

void activateAllTiles(narciaMap_t *map) {
	for (int y = 0; y < map->mapSize; y++) {
		for (int x = 0; x < map->mapSize; x++) {
			map->map[y][x].active = true;
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

	activateAllTiles(narciaMap);

	narciaMap->paths = DynamicArray_init(10);

	return narciaMap;
}

void narciaMapAddPath(narciaMap_t *map, path_t *path) { DynamicArray_Add(map->paths, path); }