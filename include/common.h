#ifndef COMMON_H
#define COMMON_H

#include "DynamicArray.h"
#include "config.h"
#include "coreWindowState.h"
#include <windows.h>

extern WindowState_t currentWindowState;

typedef struct InputWidget inputWidget_t;
typedef struct textDumpWidget textDumpWidget_t;

extern inputWidget_t *activeInput;
extern textDumpWidget_t *activeTextDump;

typedef struct CommonPos {
	float top;    // spacing from top
	float left;   // spacing from left
	float right;  // distance from left to right edge (so: width = right - left)
	float bottom; // distance from top to bottom edge (so: height = bottom - top)
} CommonPos_t;

typedef struct AbsolutePos {
	int top;
	int left;
	int right;
	int bottom;
} AbsolutePos_t;

typedef enum posType {
	POS_TYPE_REL,
	POS_TYPE_ABS,
	POS_TYPE_ANCHOR,
} posType_t;

#ifndef GET_X_PARAM
#define GET_X_PARAM(lp) ((int)(short)LOWORD(lp))
#endif

#ifndef GET_Y_PARAM
#define GET_Y_PARAM(lp) ((int)(short)HIWORD(lp))
#endif

#endif