#ifndef CALENDAR_ENTRY_H
#define CALENDAR_ENTRY_H

#include "common.h"
#include "container.h"

typedef enum{
    ENTRY_CHECKBOX,
    ENTRY_NUM,
} entry_type_t;

typedef union {
    bool state;
    int count;
} entry_data_t;


typedef struct {
    entry_type_t type;
    char text[64];
    entry_data_t data;
} calender_entry_t;

void renderCalendarEntries(container_t *grid, calender_entry_t *entries, int numEntries); 

#endif