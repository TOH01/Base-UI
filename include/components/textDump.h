#ifndef TEXT_DUMP_H
#define TEXT_DUMP_H

#include "common.h"
#include "widget.h"

typedef struct textLineNode textLineNode_t;

typedef struct textLineNode {
    char * line;
    textLineNode_t * nextNode;
    textLineNode_t * prevNode;
} textLineNode_t;

typedef struct textDumpWidget {
    BaseWidget_t baseWidget;
    textLineNode_t * firstLine;
    textLineNode_t * lastLine;
    textLineNode_t * currentLine;
    textDumpTheme_t * theme;
} textDumpWidget_t;

textDumpWidget_t * costumTextDump_initTextDump(CommonPos_t pos);
void costumTextDump_AddLine(textDumpWidget_t * textDump, const char * newText);

#endif