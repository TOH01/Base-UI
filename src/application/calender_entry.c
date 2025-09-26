#include <stdio.h>
#include <limits.h>
#include "calendar_entry.h"
#include "customButton.h"
#include "customCheckbox.h"
#include "customInput.h"

void renderCalendarEntries(container_t *grid, calender_entry_t *entries, int numEntries) {
    if (grid->cols < 5) return;

    int leftPadding   = 1;
    int rightPadding  = 1;
    int widgetCols    = 2;
    int spacerCols    = 1;
    int totalCols     = grid->cols;
    int labelCols     = totalCols - (leftPadding + widgetCols + spacerCols + rightPadding);

    int currentRow = 0;

    currentRow++;

    for (int i = 0; i < numEntries; i++, currentRow++) {
        calender_entry_t *entry = &entries[i];

        BaseWidget_t *widget = NULL;
        switch (entry->type) {
            case ENTRY_CHECKBOX:
                widget = (BaseWidget_t*) customCheckbox_initCheckbox(
                    (CommonPos_t){0,0,0,0}, &entry->data.state
                );
                break;

            case ENTRY_NUM: {
                inputWidget_t *input = customInput_initInput((CommonPos_t){0,0,0,0});
                char buf[32];
                snprintf(buf, sizeof(buf), "%d", entry->data.count);
                widget = (BaseWidget_t*) input;
            } break;
        }

        if (widget) {
            for (int c = leftPadding; c < leftPadding + widgetCols; c++) {
                addWidgetToGridContainer(grid, widget, currentRow, c);
            }
        }

        buttonWidget_t *label = customButton_initButton((CommonPos_t){0,0,0,0}, NULL, 0);
        customButton_setButtonText(label, entry->text);

        int labelStart = leftPadding + widgetCols + spacerCols;
        int labelEnd   = labelStart + labelCols;
        for (int c = labelStart; c < labelEnd; c++) {
            addWidgetToGridContainer(grid, (BaseWidget_t*)label, currentRow, c);
        }

        currentRow++;
    }

    grid->rows = currentRow; 

    updateGridPositions(grid);
	updateWidgetVisibility(); // TODO: ONLY UPDATE THIS CONTAINER
 
    InvalidateRect(currentWindowState.hwnd, NULL, FALSE);
}