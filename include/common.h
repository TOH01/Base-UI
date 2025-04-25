#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include "coreWindowState.h"
#include <windows.h>

extern WindowState_t currentWindowState;

typedef struct InputWidget inputWidget_t;
typedef struct textDumpWidget textDumpWidget_t;

extern inputWidget_t *activeInput;
extern textDumpWidget_t *activeTextDump;

typedef struct CommonPos {
	float top;
	float left;
	float right;
	float bottom;
} CommonPos_t;

#endif