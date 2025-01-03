#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include "config.h"
#include "coreWindowState.h"

extern WindowState_t currentWindowState;

typedef struct CoomonPos{
    int spacingTop;
    int spacingLeft;
    int width;
    int height;
} CommonPos_t;

#endif