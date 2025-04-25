#ifndef COSTUM_CHECKBOX_H
#define COSTUM_CHECKBOX_H

#include "common.h"
#include "widget.h"

typedef struct CheckboxWidget {
	BaseWidget_t baseWidget;
	bool *value;
	CheckboxTheme_t *theme;
} checkboxWidget_t;

checkboxWidget_t *customCheckbox_initCheckbox(CommonPos_t pos, bool *value);

#endif