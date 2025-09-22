#include "common.h"
#include "calendar.h"
#include "UiUtils.h"
#include <time.h>
#include <stdio.h>
#include <math.h>

// Helper: get days in month
static int getDaysInMonth(int year, int month) {
    static int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int d = days[month - 1];
    if (month == 2) {
        // leap year
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            d = 29;
    }
    return d;
}

// Helper: weekday of 1st of month (0=Sunday)
static int getStartWeekday(int year, int month) {
    SYSTEMTIME st = {0};
    st.wYear = year;
    st.wMonth = month;
    st.wDay = 1;
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);

    FILETIME localFt;
    FileTimeToLocalFileTime(&ft, &localFt);

    SYSTEMTIME lt;
    FileTimeToSystemTime(&localFt, &lt);

    return lt.wDayOfWeek;
}

static void drawCalendar(BaseWidget_t *base) {
    calendarWidget_t *cal = (calendarWidget_t *)base;

    const int cols = 7;
    const int rows = 6;

    int width  = base->pos.right  - base->pos.left;
    int height = base->pos.bottom - base->pos.top;

    // floating-point step sizes
    double cellWidthF  = (double)width  / cols;
    double cellHeightF = (double)height / rows;

    COLORREF borderColor   = RGB(200,200,200);
    COLORREF todayColor    = RGB(200,230,255);
    COLORREF selectedColor = RGB(180,220,180);

    int day = 1;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            // calculate cell edges smoothly
            AbsolutePos_t cellPos;
            cellPos.top    = base->pos.top + (int)round(row    * cellHeightF);
            cellPos.bottom = base->pos.top + (int)round((row+1)* cellHeightF);
            cellPos.left   = base->pos.left+ (int)round(col    * cellWidthF);
            cellPos.right  = base->pos.left+ (int)round((col+1)* cellWidthF);

            int dayNum = -1;
            int idx = row * cols + col;
            if (idx >= cal->startWeekday && day <= cal->daysInMonth) {
                dayNum = day++;
            }

            COLORREF fill = RGB(255,255,255);
            if (dayNum > 0) {
                if (dayNum == cal->todayDay &&
                    cal->month == cal->todayMonth &&
                    cal->year  == cal->todayYear) {
                    fill = todayColor;
                }
                if (dayNum == cal->selectedDay) {
                    fill = selectedColor;
                }
            }

            UiUtils_DrawColoredRectangle(cellPos, fill, borderColor, 1);

            if (dayNum > 0) {
                char buf[12];
                sprintf(buf, "%d", dayNum);
                UiUtils_DrawTextTheme(cellPos, buf, currentWindowState.activeTheme.button.text.formatFlags, currentWindowState.activeTheme.button.text.font,currentWindowState.activeTheme.button.text.color);
            }
        }
    }
}

static void onCalendarClick(BaseWidget_t *base, int x, int y, ClickType_t clickType) {
    if (clickType != CLICK_TYPE_LUP) return;

    calendarWidget_t *cal = (calendarWidget_t *)base;

    int cols = 7;
    int rows = 6;
    int cellWidth = (base->pos.right - base->pos.left) / cols;
    int cellHeight = (base->pos.bottom - base->pos.top) / rows;

    int relX = x - base->pos.left;
    int relY = y - base->pos.top;

    int col = relX / cellWidth;
    int row = relY / cellHeight;
    int idx = row * cols + col;

    int dayNum = idx - cal->startWeekday + 1;
    if (dayNum > 0 && dayNum <= cal->daysInMonth) {
        cal->selectedDay = dayNum;
        
        if(cal->dateChangeCallback != NULL){      
            cal->dateChangeCallback();
        }

        InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
    }
}

void updateVisibleCalendar(calendarWidget_t * cal, int year, int month){
    cal->year = year;
    cal->month = month;
    cal->daysInMonth = getDaysInMonth(year, month);
    cal->startWeekday = getStartWeekday(year, month);
    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}

calendarWidget_t *initCalendarWidget(CommonPos_t pos, int year, int month) {
    calendarWidget_t *cal = calloc(1, sizeof(calendarWidget_t));
    cal->baseWidget.initPos = pos;
    cal->baseWidget.drawHandler = &drawCalendar;
    cal->baseWidget.onClick = &onCalendarClick;

    // set month/year
    cal->year = year;
    cal->month = month;
    cal->daysInMonth = getDaysInMonth(year, month);
    cal->startWeekday = getStartWeekday(year, month);

    // today’s date
    SYSTEMTIME st;
    GetLocalTime(&st);
    cal->todayDay = st.wDay;
    cal->todayMonth = st.wMonth;
    cal->todayYear = st.wYear;

    cal->selectedDay = -1;
    return cal;
}