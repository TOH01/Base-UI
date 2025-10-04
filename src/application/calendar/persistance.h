#ifndef PERSISTANCE_H
#define PERSISTANCE_H

#include "calendar_entry.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 0
#define COUNT_VERSION 1

#define MAGIC 0x52444C43u  // CLDR

#define NULL_TERMINATOR_BYTE 1

typedef struct version{
    unsigned char major;
    unsigned char minor;
    unsigned short counter;
} version_t;

typedef struct file_header{
    unsigned int magic;
    version_t version;
    int last_offset;
    int latest_rule_id;
} file_header_t;

typedef struct idx_entry{
    int key; 
    int offset;
} idx_entry_t;

typedef struct day_save_data{
    int elements;
    calender_entry_t * entries;
} day_save_data_t;

typedef struct {
    int rule_id;
    int start_year;
    int start_month;
    int start_day;
    int end_year;
    int end_month;
    int end_day;
    entry_type_t type;
    char data[256]; 
} rule_t;

typedef struct {
    entry_type_t deletedFlag;
    int ruleId;
} deleted_day_t;

void create_file_system(void);
void saveDay(day_save_data_t *day, int day_num, int month, int year);
day_save_data_t *loadDay(int day_num, int month, int year);
void overwriteDayData(day_save_data_t *day, int day_num, int month, int year);
void save_rule(rule_t *rule, const char *data_file, const char *idx_file);
day_save_data_t *loadDayWithRules(int day_num, int month, int year);

#endif