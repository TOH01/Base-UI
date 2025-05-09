#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include "coreWindowState.h"
#include "DynamicArray.h"
#include <windows.h>

extern WindowState_t currentWindowState;

typedef struct InputWidget inputWidget_t;
typedef struct textDumpWidget textDumpWidget_t;

extern inputWidget_t *activeInput;
extern textDumpWidget_t *activeTextDump;

typedef struct CommonPos {
    float top;     // spacing from top
    float left;    // spacing from left
    float right;   // distance from left to right edge (so: width = right - left)
    float bottom;  // distance from top to bottom edge (so: height = bottom - top)
} CommonPos_t;

#endif