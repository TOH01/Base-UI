#ifndef TEXT_DUMP_H
#define TEXT_DUMP_H

#include "common.h"
#include "widget.h"

typedef struct textDumpWidget {
    BaseWidget_t baseWidget;
    char * textBuffer;
    float currentYOffset;
} textDumpWidget_t;

#endif

textDumpWidget_t * initTextDump(CommonPos_t pos, char * textBuffer);