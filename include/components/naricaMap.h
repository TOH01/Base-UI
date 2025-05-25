#ifndef NARICA_MAP_H
#define NARICA_MAP_H

#include "common.h"
#include "widget.h"

#define MAX_MAP_SIZE 200 // with 2 current narcia gamemodes, max map is 200 big

typedef enum tileType {
	TILE_EMPTY,
	TILE_TOWN,
	TILE_TOWN_CENTER,
	SELECTED,
} tileType_t;

typedef enum townType{
	TOWN_TYPE_LARGE,
	TOWN_TYPE_SMALL,
	TOWN_TYPE_IMPERIAL_CASTLE,
} townType_t;

typedef struct mapTile{
	tileType_t type;
	townType_t townType;
	int pathID;
} mapTile_t;

typedef struct path{
	DynamicArray_t * tiles;
	COLORREF color;
} path_t;

typedef struct Coordinate {
	int x;
	int y;
} Coordinate_t;

typedef struct narciaMap {
	BaseWidget_t baseWidget;
	int middleX;
	int middleY;
	int tileSize;
	int mapSize;	// size of the map, 180 - war era, 200 - narica 2.0 (width and height always same)
	mapTile_t map[MAX_MAP_SIZE][MAX_MAP_SIZE];
	int zoomStep;
	int minTileSize;
	int maxTileSize;
	DynamicArray_t * paths;
} narciaMap_t;

narciaMap_t *initNarciaMap(CommonPos_t pos);

static const Coordinate_t narciaWarEraTowns[191] = {
    {9, 162}, {10, 17}, {12, 12}, {12, 23}, {12, 41}, {12, 59}, {12, 77}, {12, 95}, {12, 113}, {12, 131}, {12, 149}, {12, 167}, {13, 30}, {13, 48}, {13, 66}, {13, 84}, {13, 102}, {13, 120}, {13, 138}, {13, 156}, {17, 8}, {17, 170}, {19, 18}, {20, 39}, {20, 52}, {23, 12}, {23, 27}, {23, 167}, {24, 153}, {26, 61}, {26, 79}, {26, 99}, {26, 140}, {27, 38}, {27, 116}, {27, 125}, {28, 108}, {29, 132}, {30, 13}, {30, 51}, {30, 166}, {31, 23}, {32, 66}, {32, 94}, {36, 156}, {38, 32}, {38, 114}, {38, 142}, {39, 20}, {40, 82}, {40, 132}, {41, 12}, {41, 167}, {42, 64}, {43, 43}, {44, 52}, {44, 99}, {44, 155}, {46, 24}, {47, 144}, {48, 13},  {48, 166}, {52, 70}, {52, 89}, {52, 109}, {53, 33}, {55, 52}, {55, 141}, {56, 20}, {56, 41}, {56, 123}, {57, 79}, {59, 12}, {59, 92}, {59, 155}, {59, 167}, {61, 61}, {63, 28}, {64, 109}, {65, 131}, {66, 13}, {66, 166}, {67, 39}, {67, 149}, {69, 21},  {72, 73}, {72, 140}, {76, 52}, {77, 12}, {77, 167}, {78, 27}, {78, 156}, {79, 124}, {82, 20}, {82, 37}, {83, 63}, {84, 13}, {84, 166}, {89, 113}, {89, 155}, {90, 141}, {91, 21},  {95, 8},  {95, 167}, {97, 37}, {98, 51}, {98, 153}, {101, 27}, {101, 62}, {101, 117}, {102, 13}, {102, 166}, {104, 108}, {106, 89}, {107, 141}, {109, 21},  {110, 122}, {112, 39}, {113, 12}, {113, 154}, {113, 167}, {115, 131}, {117, 27}, {118, 71}, {118, 87}, {119, 55}, {119, 110}, {120, 13}, {120, 166}, {122, 139}, {126, 45},  {126, 123}, {128, 36}, {128, 153}, {130, 95}, {131, 12}, {131, 80}, {131, 167}, {132, 111}, {134, 22}, {134, 69}, {136, 54},  {138, 13}, {138, 42}, {139, 166}, {141, 139}, {142, 125}, {143, 152}, {147, 19}, {148, 62}, {148, 89}, {149, 12}, {149, 101}, {149, 167}, {150, 47}, {150, 73}, {151, 116}, {153, 132}, {153, 140}, {154, 149}, {155, 24}, {156, 13}, {156, 96}, {156, 166}, {157, 108}, {158, 37}, {159, 54}, {159, 70}, {160, 84}, {162, 8}, {162, 170}, {166, 23}, {166, 41}, {166, 59}, {166, 77}, {166, 95}, {166, 113}, {166, 133}, {166, 149}, {167, 12}, {167, 30}, {167, 48}, {167, 66}, {167, 84}, {167, 102}, {167, 120}, {167, 138}, {167, 156}, {167, 167}, {170, 18}, {171, 162},
};

static const Coordinate_t narciaWarEraLargeTowns[7] = {
	{109, 128}, {95, 127}, {136, 89}, {71, 95}, {91, 70}, {126, 54}, {48, 116}
};

static const Coordinate_t narciaWarEraImperalCastle[1] = {
	{89, 89}
};

static const int narciaWarEraTowns_count = 191;
static const int narciaWarEraLargeTowns_count = 7;
static const int narciaWarEraImperalCastle_count = 1;

void goToTile(narciaMap_t * narciaMap, int x, int y);

#endif