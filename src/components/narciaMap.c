#include "UiUtils.h"
#include "naricaMap.h"
#include "textDump.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

narciaMap_t *focusedNarciaMap = NULL;

narciaMap_t *activeNaricMap = NULL;
int activeDragStartX = 0;
int activeDragStartY = 0;
int dragStartMiddleX = 0;
int dragStartMiddleY = 0;

bool callbackInitialized = false;

static HBRUSH checkerBrush = NULL;
static HBITMAP checkerBitmap = NULL;
static int lastTileSize = -1;

HFONT cachedFontCoordinates = NULL;
HFONT cachedFontTownName = NULL;
char maxTextForCachedFontCoordinates[11] = "(XXX, XXX)";
char maxTextForCachedFontName[3] = "XX";

HPEN selectedTownPen = NULL;
HPEN excludedTownPen = NULL;
HPEN largeTownPen = NULL;
HPEN smallTownPen = NULL;
HPEN imperialCastlePen = NULL;

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

		UiUtils_DrawLineRelativeTheme((AbsolutePos_t){.left = fromPoint.x, .top = fromPoint.y, .right = toPoint.x, .bottom = toPoint.y}, color, 4);
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

static HPEN TileTypeToColor(mapTile_t mapTile) {
	switch (mapTile.townType) {
	case TOWN_TYPE_LARGE:
		return largeTownPen;
	case TOWN_TYPE_IMPERIAL_CASTLE:
		return imperialCastlePen;
	case TOWN_TYPE_SMALL:
		return smallTownPen;
	default:
		return NULL;
	}
}

bool adjacentToTown(Coordinate_t townCenter, Coordinate_t tile) { return (abs(townCenter.x - tile.x) <= 2) && (abs(townCenter.y - tile.y) <= 2); }

static void drawTown(AbsolutePos_t pos, mapTile_t mapTile) {

	switch (mapTile.type) {
	case TILE_TOWN_TOP:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.right, pos.top});
		break;

	case TILE_TOWN_BOTTOM:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.bottom, pos.left, pos.right, pos.bottom});
		break;

	case TILE_TOWN_LEFT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.left, pos.bottom});
		break;

	case TILE_TOWN_RIGHT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.right, pos.right, pos.bottom});
		break;

	case TILE_TOWN_TOP_LEFT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.right, pos.top});
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.left, pos.bottom});
		break;

	case TILE_TOWN_TOP_RIGHT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.right, pos.top});
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.right, pos.right, pos.bottom});
		break;

	case TILE_TOWN_BOTTOM_LEFT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.bottom, pos.left, pos.right, pos.bottom});
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.left, pos.left, pos.bottom});
		break;

	case TILE_TOWN_BOTTOM_RIGHT:
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.bottom, pos.left, pos.right, pos.bottom});
		UiUtils_DrawLineRelative((AbsolutePos_t){pos.top, pos.right, pos.right, pos.bottom});
		break;

	case TILE_TOWN_CENTER:
		// Optionally, draw a cross or leave it blank
		break;

	default:
		break;
	}
}

bool coordinateEqual(Coordinate_t c1, Coordinate_t c2) { return (c1.x == c2.x && c1.y == c2.y); }

void drawTownCoordinates(Coordinate_t coordinate, AbsolutePos_t rect) {
	char text[11]; // (XXX, XXX) + string terminator
	snprintf(text, 11, "(%d, %d)", coordinate.x, coordinate.y);

	UiUtils_DrawText(rect, text, DT_CENTER | DT_VCENTER | DT_NOCLIP);
}

void drawTownName(townType_t type, AbsolutePos_t rect) {
	char text[3] = ""; // only "IC" and "LT" valid + string terminator

	if (type == TOWN_TYPE_LARGE) {
		strncpy(text, "LT", 2);
		text[2] = '\0';
	} else if (type == TOWN_TYPE_IMPERIAL_CASTLE) {
		strncpy(text, "IC", 2);
		text[2] = '\0';
	}

	UiUtils_DrawText(rect, text, DT_CENTER | DT_VCENTER | DT_NOCLIP);
}

static void drawNarciaMap(BaseWidget_t *base) {
	narciaMap_t *map = (narciaMap_t *)base;
	const int tileSize = map->tileSize;
	const int mapSize = map->mapSize;

	const int left = base->pos.left;
	const int top = base->pos.top;
	const int right = base->pos.right;
	const int bottom = base->pos.bottom;

	const int widgetWidth = right - left;
	const int widgetHeight = bottom - top;

	const int centerX = left + widgetWidth / 2;
	const int centerY = top + widgetHeight / 2;

	const int middleTileTopLeftX = centerX - tileSize / 2;
	const int middleTileTopLeftY = centerY - tileSize / 2;

	const float tilesLeftF = ((float)(centerX - left) + tileSize / 2.0f) / tileSize;
	const float tilesUpF = ((float)(centerY - top) + tileSize / 2.0f) / tileSize;

	int startX = map->middleX - (int)ceil(tilesLeftF) + 1;
	int endX = map->middleX + (int)ceil(tilesLeftF);
	int startY = map->middleY - (int)ceil(tilesUpF) + 1;
	int endY = map->middleY + (int)ceil(tilesUpF);

	if (startX < 0)
		startX = 0;
	if (endX >= mapSize)
		endX = mapSize - 1;
	if (startY < 0)
		startY = 0;
	if (endY >= mapSize)
		endY = mapSize - 1;

	// --- Calculate tile drawing bounds in screen px ---
	const int tileDrawLeft = middleTileTopLeftX + (startX - map->middleX) * tileSize;
	const int tileDrawTop = middleTileTopLeftY + (startY - map->middleY) * tileSize;
	const int tileDrawRight = middleTileTopLeftX + (endX - map->middleX + 1) * tileSize;
	const int tileDrawBottom = middleTileTopLeftY + (endY - map->middleY + 1) * tileSize;

	AbsolutePos_t backgroundPos = {.left = max(tileDrawLeft, left), .top = max(tileDrawTop, top), .right = min(tileDrawRight, right), .bottom = min(tileDrawBottom, bottom)};
	RECT backgroundRect = UiUtils_absolutePosToRect(backgroundPos);

	// --- Rebuild brush if tile size changed ---
	if (lastTileSize != tileSize) {
		lastTileSize = tileSize;

		if (checkerBrush) {
			DeleteObject(checkerBrush);
			checkerBrush = NULL;
		}
		if (checkerBitmap) {
			DeleteObject(checkerBitmap);
			checkerBitmap = NULL;
		}

		const int patternSize = tileSize * 2;

		HDC memDC = CreateCompatibleDC(currentWindowState.memHDC);
		checkerBitmap = CreateCompatibleBitmap(currentWindowState.memHDC, patternSize, patternSize);
		HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, checkerBitmap);

		// Fill with base color
		HBRUSH baseBrush = CreateSolidBrush(RGB(140, 182, 58));
		FillRect(memDC, &(RECT){0, 0, patternSize, patternSize}, baseBrush);
		DeleteObject(baseBrush);

		// Draw dark squares
		HBRUSH darkBrush = CreateSolidBrush(RGB(115, 157, 47));
		FillRect(memDC, &(RECT){0, 0, tileSize, tileSize}, darkBrush);
		FillRect(memDC, &(RECT){tileSize, tileSize, patternSize, patternSize}, darkBrush);
		DeleteObject(darkBrush);

		SelectObject(memDC, oldBmp);
		DeleteDC(memDC);

		checkerBrush = CreatePatternBrush(checkerBitmap);
	}

	// --- Align brush to tiles ---
	int brushOrgX = (tileDrawLeft) % (tileSize * 2);
	int brushOrgY = (tileDrawTop) % (tileSize * 2);
	if (brushOrgX < 0)
		brushOrgX += tileSize * 2;
	if (brushOrgY < 0)
		brushOrgY += tileSize * 2;
	SetBrushOrgEx(currentWindowState.memHDC, brushOrgX, brushOrgY, NULL);

	// --- Draw checker background ---
	HBRUSH oldBrush = (HBRUSH)SelectObject(currentWindowState.memHDC, checkerBrush);
	FillRect(currentWindowState.memHDC, &backgroundRect, checkerBrush);
	SelectObject(currentWindowState.memHDC, oldBrush);

	cachedFontCoordinates = NULL;
	cachedFontTownName = NULL;

	SetTextColor(currentWindowState.memHDC, RGB(0, 0, 0));
	SetBkMode(currentWindowState.memHDC, TRANSPARENT);

	bool selected1Valid = (map->selected1.x >= 0 && map->selected1.y >= 0);
	bool selected2Valid = (map->selected2.x >= 0 && map->selected2.y >= 0);

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

			if (selected1Valid && mapTile.townID == map->map[map->selected1.y][map->selected1.x].townID)
				selected = true;
			if (selected2Valid && mapTile.townID == map->map[map->selected2.y][map->selected2.x].townID)
				selected = true;

			Coordinate_t townCenter = getCenterOfTownTile(map, (Coordinate_t){x, y});
			bool disabled = !map->map[townCenter.y][townCenter.x].active;

			if (disabled) {
				HPEN oldPen = SelectObject(currentWindowState.memHDC, excludedTownPen);
				drawTown(rect, mapTile);
				SelectObject(currentWindowState.memHDC, oldPen);
			} else if (selected) {
				HPEN oldPen = SelectObject(currentWindowState.memHDC, selectedTownPen);
				drawTown(rect, mapTile);
				SelectObject(currentWindowState.memHDC, oldPen);
			} else {
				HPEN oldPen = SelectObject(currentWindowState.memHDC, TileTypeToColor(mapTile));
				drawTown(rect, mapTile);
				SelectObject(currentWindowState.memHDC, oldPen);
			}

			if (mapTile.type == TILE_TOWN_TOP_LEFT) {
				rect.bottom = rect.top;
				rect.top -= map->tileSize * 1.5;
				rect.right += map->tileSize * 2.5;
				rect.left -= map->tileSize / 2;

				if (!cachedFontCoordinates) {
					cachedFontCoordinates = getFontForRect(currentWindowState.memHDC, maxTextForCachedFontCoordinates, UiUtils_absolutePosToRect(rect));
				}

				HFONT oldFont = SelectObject(currentWindowState.memHDC, cachedFontCoordinates);

				drawTownCoordinates(townCenter, rect);

				SelectObject(currentWindowState.memHDC, oldFont);
			}
			if ((mapTile.townType == TOWN_TYPE_IMPERIAL_CASTLE || mapTile.townType == TOWN_TYPE_LARGE) && mapTile.type == TILE_TOWN_BOTTOM) {
				rect.top = rect.bottom;
				rect.bottom += map->tileSize * 1.5;
				rect.left -= map->tileSize * 1.5;
				rect.right += map->tileSize * 1.5;

				if (!cachedFontTownName) {
					cachedFontTownName = getFontForRect(currentWindowState.memHDC, maxTextForCachedFontName, UiUtils_absolutePosToRect(rect));
				}

				HFONT oldFont = SelectObject(currentWindowState.memHDC, cachedFontTownName);

				drawTownName(mapTile.townType, rect);

				SelectObject(currentWindowState.memHDC, oldFont);
			}
		}
	}

	if (map->paths) {
		int pathCount = map->paths->size;
		for (int i = 0; i < pathCount; i++) {
			path_t *path = (path_t *)DynamicArray_get(map->paths, i);
			if (!path || !path->tiles || path->tileCount < 2)
				continue;

			if (path == map->selecetdPath) {
				drawPathOnMap(map, RGB(255, 255, 255), path->tiles, path->tileCount);
			} else {
				drawPathOnMap(map, path->color, path->tiles, path->tileCount);
			}
		}
	}
}

void RbuttonDownCallbackNaricaMap(int x, int y, narciaMap_t *map) {
	Coordinate_t tile;
	if (screenPosToNarciaPos(map, x, y, &tile)) {
		mapTile_t clickedTile = map->map[tile.y][tile.x];
		if (clickedTile.townID != 0 && !coordinateEqual(tile, map->selected1) && !coordinateEqual(tile, map->selected2)) {
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
	(void)wParam;
	(void)lParam;
	(void)msg;

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

				if (coordinateEqual(selected1, townCenter)) {
					map->selected1 = (Coordinate_t){-1, -1};
				} else if (coordinateEqual(selected2, townCenter)) {
					map->selected2 = (Coordinate_t){-1, -1};
				} else if (selected1.x == -1) {
					map->selected1 = townCenter;
				} else if (selected2.x == -1) {
					map->selected2 = townCenter;
				} else {
					map->selected1 = selected2;
					map->selected2 = townCenter;
				}
			}
		}
	}

	activeNaricMap = NULL;
	return 0;
}

LRESULT handleMouseWheelNarcia(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)lParam;
	(void)hwnd;
	(void)msg;

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

	return 0;
}

void goToTile(narciaMap_t *narciaMap, int x, int y) {
	narciaMap->middleX = x;
	narciaMap->middleY = y;
	InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

LRESULT mouseMoveNarciaMap(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	(void)msg;
	(void)wParam;
	(void)hwnd;

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

void PopulateMapWithTowns(narciaMap_t *map, const Coordinate_t *coords, int coordCount, townType_t townType, int startID) {

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
	narciaMap->maxTileSize = 35;
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

	if (!selectedTownPen) {
		selectedTownPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
		excludedTownPen = CreatePen(PS_SOLID, 3, RGB(184, 184, 184));
		largeTownPen = CreatePen(PS_SOLID, 3, RGB(123, 77, 247));
		smallTownPen = CreatePen(PS_SOLID, 3, RGB(148, 89, 247));
		imperialCastlePen = CreatePen(PS_SOLID, 3, RGB(181, 20, 247));
	}

	return narciaMap;
}

void narciaMapAddPath(narciaMap_t *map, path_t *path) { DynamicArray_Add(map->paths, path); }

float getDistance(Coordinate_t c1, Coordinate_t c2) {
	int dx = c2.x - c1.x;
	int dy = c2.y - c1.y;
	return sqrtf((float)(dx * dx + dy * dy));
}

int getTimeForDistance(float distance) { return (uint32_t)(1.108f * powf(distance, 1.926f)); }

int getWaterForDistance(float distance) { return (uint32_t)(54.0 * getTimeForDistance(distance)); }

DynamicArray_t *getAllActiveTown(narciaMap_t *map) {

	DynamicArray_t *array = DynamicArray_init(narciaWarEraTowns_count + narciaWarEraLargeTowns_count + narciaWarEraImperalCastle_count);

	for (int y = 0; y < map->mapSize; y++) {
		for (int x = 0; x < map->mapSize; x++) {
			if (map->map[y][x].type == TILE_TOWN_CENTER && map->map[y][x].active) {
				Coordinate_t *coordinate = (Coordinate_t *)malloc(sizeof(Coordinate_t));
				coordinate->y = y;
				coordinate->x = x;
				DynamicArray_Add(array, coordinate);
			}
		}
	}

	return array;
}

path_t *findShortestPath(narciaMap_t *map, Coordinate_t start, Coordinate_t end) {
	DynamicArray_t *allTowns = getAllActiveTown(map);
	int townCount = allTowns->size;
	if (townCount > MAX_TOWN_COUNT) {
		DynamicArray_Free(allTowns);
		return NULL;
	}

	// Allocate graph on heap
	float *graph = malloc(sizeof(float) * townCount * townCount);
	if (!graph) {
		DynamicArray_Free(allTowns);
		return NULL;
	}

#define GRAPH(i, j) graph[(i) * townCount + (j)]

	// Build graph
	for (int i = 0; i < townCount; i++) {
		for (int j = 0; j < townCount; j++) {
			if (i == j) {
				GRAPH(i, j) = 0.0f;
				continue;
			}
			Coordinate_t *a = (Coordinate_t *)allTowns->items[i];
			Coordinate_t *b = (Coordinate_t *)allTowns->items[j];
			float dist = getDistance(*a, *b);
			GRAPH(i, j) = (dist < MAX_DISTANCE) ? (float)getTimeForDistance(dist) : INF_TIME;
		}
	}

	// Find start and end indices
	int startIndex = -1, endIndex = -1;
	for (int i = 0; i < townCount; i++) {
		Coordinate_t *t = (Coordinate_t *)allTowns->items[i];
		if (t->x == start.x && t->y == start.y)
			startIndex = i;
		if (t->x == end.x && t->y == end.y)
			endIndex = i;
	}

	if (startIndex == -1 || endIndex == -1) {
		free(graph);
		DynamicArray_Free(allTowns);
		return NULL;
	}

	// Dijkstra setup
	TownNode *nodes = calloc(townCount, sizeof(TownNode));
	for (int i = 0; i < townCount; i++) {
		nodes[i].coord = *(Coordinate_t *)allTowns->items[i];
		nodes[i].time = INF_TIME;
		nodes[i].previousIndex = -1;
		nodes[i].visited = false;
	}
	nodes[startIndex].time = 0;

	// Dijkstra's algorithm
	for (int count = 0; count < townCount; count++) {
		int u = -1;
		float minTime = INF_TIME;
		for (int i = 0; i < townCount; i++) {
			if (!nodes[i].visited && nodes[i].time < minTime) {
				minTime = nodes[i].time;
				u = i;
			}
		}
		if (u == -1)
			break;

		nodes[u].visited = true;

		for (int v = 0; v < townCount; v++) {
			if (!nodes[v].visited && GRAPH(u, v) < INF_TIME) {
				float alt = nodes[u].time + GRAPH(u, v);
				if (alt < nodes[v].time) {
					nodes[v].time = alt;
					nodes[v].previousIndex = u;
				}
			}
		}
	}

	// Reconstruct path
	if (nodes[endIndex].previousIndex == -1) {
		free(nodes);
		free(graph);
		DynamicArray_Free(allTowns);
		return NULL;
	}

	int pathSize = 0;
	for (int i = endIndex; i != -1; i = nodes[i].previousIndex)
		pathSize++;

	Coordinate_t *tileList = malloc(sizeof(Coordinate_t) * pathSize);
	int index = pathSize - 1;
	for (int i = endIndex; i != -1; i = nodes[i].previousIndex)
		tileList[index--] = nodes[i].coord;

	path_t *path = malloc(sizeof(path_t));
	path->tiles = tileList;
	path->tileCount = pathSize;
	path->color = RGB(255, 255, 0); // Yellow path

	free(nodes);
	free(graph);
	DynamicArray_Free(allTowns);
	return path;
}

void pathDistanceToTextDump(path_t *path, textDumpWidget_t *textDump) {
	customTextDump_AddLine(textDump, "Distance for path:");

	//(XXX, XXX) -> (XXX, XXX) - XXXXXXX
	char buff[40] = "";

	Coordinate_t c1;
	Coordinate_t c2;
	float totalDistance = 0;

	for (int i = 0; i < path->tileCount - 1; i++) {
		c1 = (Coordinate_t){path->tiles[i].x, path->tiles[i].y};
		c2 = (Coordinate_t){path->tiles[i + 1].x, path->tiles[i + 1].y};

		totalDistance += getDistance(c1, c2);
		snprintf(buff, sizeof(buff), "(%d, %d) -> (%d, %d) - %.1f", c1.x, c1.y, c2.x, c2.y, getDistance(c1, c2));
		customTextDump_AddLine(textDump, buff);
	}

	snprintf(buff, sizeof(buff), "Total distance - %.1f", totalDistance);
	customTextDump_AddLine(textDump, buff);
	customTextDump_AddLine(textDump, "------------------------------");
	customTextDump_AddLine(textDump, "");
}

void pathWaterToTextDump(path_t *path, textDumpWidget_t *textDump) {
	customTextDump_AddLine(textDump, "Water cost (3 heroes) for path:");

	//(XXX, XXX) -> (XXX, XXX) - XXXXXXX
	char buff[40] = "";

	Coordinate_t c1;
	Coordinate_t c2;
	int totalWater = 0;

	for (int i = 0; i < path->tileCount - 1; i++) {
		c1 = (Coordinate_t){path->tiles[i].x, path->tiles[i].y};
		c2 = (Coordinate_t){path->tiles[i + 1].x, path->tiles[i + 1].y};

		totalWater += getWaterForDistance(getDistance(c1, c2));
		snprintf(buff, sizeof(buff), "(%d, %d) -> (%d, %d) - %d", c1.x, c1.y, c2.x, c2.y, getWaterForDistance(getDistance(c1, c2)));
		customTextDump_AddLine(textDump, buff);
	}

	snprintf(buff, sizeof(buff), "Total water cost - %d", totalWater);
	customTextDump_AddLine(textDump, buff);
	customTextDump_AddLine(textDump, "------------------------------");
	customTextDump_AddLine(textDump, "");
}

void pathTimeToTextDump(path_t *path, textDumpWidget_t *textDump) {
	customTextDump_AddLine(textDump, "Time (45% speed) for path:");

	//(XXX, XXX) -> (XXX, XXX) - XXXXXXX
	char buff[40] = "";

	Coordinate_t c1;
	Coordinate_t c2;
	int totalTime = 0;

	for (int i = 0; i < path->tileCount - 1; i++) {
		c1 = (Coordinate_t){path->tiles[i].x, path->tiles[i].y};
		c2 = (Coordinate_t){path->tiles[i + 1].x, path->tiles[i + 1].y};

		int totalSeconds = getTimeForDistance(getDistance(c1, c2));
		totalTime += totalSeconds;
		int minutes = totalSeconds / 60;
		int seconds = totalSeconds % 60;

		snprintf(buff, sizeof(buff), "(%d, %d) -> (%d, %d) - %d:%02d", c1.x, c1.y, c2.x, c2.y, minutes, seconds);
		customTextDump_AddLine(textDump, buff);
	}

	snprintf(buff, sizeof(buff), "Total time - %d:%02d", totalTime / 60, totalTime % 60);
	customTextDump_AddLine(textDump, buff);
	customTextDump_AddLine(textDump, "------------------------------");
	customTextDump_AddLine(textDump, "");
}