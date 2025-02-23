#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include "config.h"
#include "coreWindowState.h"

extern WindowState_t currentWindowState;

typedef struct InputWidget inputWidget_t;
typedef struct textDumpWidget textDumpWidget_t;

extern inputWidget_t * activeInput;
extern textDumpWidget_t * activeTextDump;

typedef struct CoomonPos{
    float spacingTop;
    float spacingLeft;
    float width;
    float height;
} CommonPos_t;

#endif