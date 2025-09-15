#ifndef CALENDAR_H
#define CALENDAR_H

#include "widget.h"
#include <time.h>

typedef struct {
    BaseWidget_t baseWidget;

    int year;
    int month;   // 1–12
    int daysInMonth;
    int startWeekday; // 0=Sunday … 6=Saturday

    int todayDay;
    int todayMonth;
    int todayYear;

    int selectedDay;
} calendarWidget_t;

calendarWidget_t *initCalendarWidget(CommonPos_t pos, int year, int month);
void updateVisibleCalendar(calendarWidget_t * cal, int year, int month);

#endif