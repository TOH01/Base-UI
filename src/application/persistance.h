#ifndef PERSISTANCE_H
#define PERSISTANCE_H

#include "calendar_entry.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 0
#define COUNT_VERSION 1

#define MAGIC 0x52444C43u  // CLDR

typedef struct version{
    unsigned char major;
    unsigned char minor;
    unsigned short counter;
} version_t;


typedef struct file_header{
    unsigned int magic;
    version_t version;
} file_header_t;

void create_file_system(void);

#endif