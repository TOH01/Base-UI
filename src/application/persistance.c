#include "persistance.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RULE_DATA 255
#define INITIAL_OFFSETS_CAP 16

char *save_dir = "saves/";
char *files_names[4] = {"date.idx", "date.dat", "rules.dat", "rules.idx"};

bool dir_exists(const char *path) {
	DWORD attribs = GetFileAttributesA(path);
	return (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
}

void make_dir(const char *path) { CreateDirectoryA(path, NULL); }

bool file_exists(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (fp) {
		fclose(fp);
		return true;
	}
	return false;
}

void create_save_file(const char *filename) {
	FILE *fp = fopen(filename, "wb");
	if (!fp)
		return;

	file_header_t header = {0};
	header.magic = MAGIC;
	header.version.major = MAJOR_VERSION;
	header.version.minor = MINOR_VERSION;
	header.version.counter = COUNT_VERSION;
	header.last_offset = 0;
	header.latest_rule_id = 0;
	fwrite(&header, sizeof(file_header_t), 1, fp);
	fclose(fp);
}

void create_file_system(void) {

	if (!dir_exists(save_dir)) {
		make_dir(save_dir);
	}

	for (int i = 0; i < (int)(sizeof(files_names) / sizeof(files_names[0])); i++) {

		char *file_path = malloc(strlen(save_dir) + strlen(files_names[i]) + NULL_TERMINATOR_BYTE);
		sprintf(file_path, "%s%s", save_dir, files_names[i]);

		if (!file_exists(file_path)) {
			create_save_file(file_path);
		}

		free(file_path);
	}
}

int create_save_key(int day, int month, int year) {
	int save_key = 0;
	save_key += day;
	save_key += month * 100;
	save_key += year * 10000;
	return save_key;
}

static void save_entry(FILE *fp, const calender_entry_t *entry) {
    // write type as 1 byte
    unsigned char type = (unsigned char)entry->type;
    fwrite(&type, sizeof(type), 1, fp);

    // write string length + data
    unsigned char len = (unsigned char)strnlen(entry->text, sizeof(entry->text));
    fwrite(&len, sizeof(len), 1, fp);
    fwrite(entry->text, sizeof(char), len, fp);

    // write the rest of entry data (always 4 bytes)
    fwrite(&entry->data, sizeof(entry->data), 1, fp);
}

static void load_entry(FILE *fp, calender_entry_t *entry) {
    // read type as 1 byte
    unsigned char type;
    fread(&type, sizeof(type), 1, fp);
    entry->type = (entry_type_t)type;

    // read string length + data
    unsigned char len;
    fread(&len, sizeof(len), 1, fp);
    fread(entry->text, sizeof(char), len, fp);
    entry->text[len] = '\0'; // null-terminate

    // read rest of entry data
    fread(&entry->data, sizeof(entry->data), 1, fp);
}

/*
 *   finds the offset, to an index entry
 *   in the .idx file
 *   returns -1 if not found
 */
int find_index_entry_offset(FILE *fp, int key) {
	int result = -1; // -1 = not found

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);

	if (size >= (long)sizeof(file_header_t)) {
		int elements = (size - sizeof(file_header_t)) / sizeof(idx_entry_t);

		int left = 0;
		int right = elements - 1;

		while (left <= right) {
			int mid = left + (right - left) / 2;
			int pos = sizeof(file_header_t) + mid * sizeof(idx_entry_t);

			idx_entry_t entry;
			fseek(fp, pos, SEEK_SET);
			if (fread(&entry, sizeof(entry), 1, fp) != 1) {
				break; // read error
			}

			if (entry.key == key) {
				result = pos;     // return position in index file
				left = right + 1; // exit loop
			} else if (entry.key < key) {
				left = mid + 1;
			} else {
				right = mid - 1;
			}
		}
	}

	return result;
}

/*
 *   returns the offset for a given key, which points to data
 *   in the . dat file
 *   returns -1 if not found
 */
int get_day_data_offset(int key, char *filename) {
	FILE *fp = fopen(filename, "rb");

	int offset = -1; // default if not found

	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		long size = ftell(fp);

		if (size >= (long)sizeof(file_header_t)) {
			int elements = (size - sizeof(file_header_t)) / sizeof(idx_entry_t);

			int left = 0;
			int right = elements - 1;

			while (left <= right) {
				int mid = left + (right - left) / 2;
				long pos = sizeof(file_header_t) + mid * sizeof(idx_entry_t);

				idx_entry_t entry;
				fseek(fp, pos, SEEK_SET);
				if (fread(&entry, sizeof(entry), 1, fp) == 1) {
					if (entry.key == key) {
						offset = entry.offset;
						left = right + 1; // force exit
					} else if (entry.key < key) {
						left = mid + 1;
					} else {
						right = mid - 1;
					}
				} else {
					left = right + 1; // exit loop on read error
				}
			}
		}

		fclose(fp);
	}

	return offset;
}

void write_save_idx(int key, int offset, char *filename) {
	FILE *fp = fopen(filename, "rb+");

	idx_entry_t entry = (idx_entry_t){.key = key, .offset = offset};

	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);

		/* first entry in file (append end of file) */
		if (size < (int)(sizeof(file_header_t) + sizeof(idx_entry_t))) {
			fwrite(&entry, sizeof(idx_entry_t), 1, fp);
		} else {

			idx_entry_t last_entry;

			fseek(fp, -(int)sizeof(idx_entry_t), SEEK_END);
			fread(&last_entry, sizeof(idx_entry_t), 1, fp);

			/* new day append (end of file) */
			if (entry.key > last_entry.key) {
				fseek(fp, 0, SEEK_END);
				fwrite(&entry, sizeof(idx_entry_t), 1, fp);
			} else {
				int day_offset = find_index_entry_offset(fp, key);
				/* idx already exists (overwrite) */
				if (day_offset != -1) {
					fseek(fp, day_offset, SEEK_SET);
					fwrite(&entry, sizeof(idx_entry_t), 1, fp);
				}
				/* past day insert (middle of file)*/
				else {
					int elements = (size - sizeof(file_header_t)) / sizeof(idx_entry_t);
					int left = 0;
					int right = elements - 1;
					int insert_index = 0;

					while (left <= right) {
						int mid = left + (right - left) / 2;
						long pos = sizeof(file_header_t) + mid * sizeof(idx_entry_t);

						idx_entry_t mid_entry;
						fseek(fp, pos, SEEK_SET);
						fread(&mid_entry, sizeof(idx_entry_t), 1, fp);

						if (mid_entry.key < key) {
							left = mid + 1;
						} else {
							right = mid - 1;
						}
					}
					insert_index = left; // left is the insertion index

					long insert_pos = sizeof(file_header_t) + insert_index * sizeof(idx_entry_t);

					// shift entries after insert_pos one slot back
					for (long i = size - sizeof(idx_entry_t); i >= insert_pos; i -= sizeof(idx_entry_t)) {
						idx_entry_t temp;
						fseek(fp, i, SEEK_SET);
						fread(&temp, sizeof(idx_entry_t), 1, fp);

						fseek(fp, i + sizeof(idx_entry_t), SEEK_SET);
						fwrite(&temp, sizeof(idx_entry_t), 1, fp);
					}

					// write new entry
					fseek(fp, insert_pos, SEEK_SET);
					fwrite(&entry, sizeof(idx_entry_t), 1, fp);
				}
			}
		}

		fclose(fp);
	}
}

int save_day_data_raw(day_save_data_t *day, const char *data_filename) {
	FILE *fp = fopen(data_filename, "rb+");
	if (!fp)
		return -1;

	fseek(fp, 0, SEEK_END);
	int offset = ftell(fp);

	fwrite(&day->elements, sizeof(int), 1, fp);
	for (int i = 0; i < day->elements; i++) {
		save_entry(fp, &day->entries[i]);
	}

	file_header_t header;

	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(file_header_t), 1, fp);

	header.last_offset = offset;

	fseek(fp, 0, SEEK_SET);
	fwrite(&header, sizeof(file_header_t), 1, fp);

	fclose(fp);
	return offset;
}

int get_last_offset(const char *filename) {
	FILE *fp = fopen(filename, "rb+");

	file_header_t header;

	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(file_header_t), 1, fp);

	return header.last_offset;
}

int save_day_data_raw_at(day_save_data_t *day, const char *data_filename, int offset) {
	FILE *fp = fopen(data_filename, "rb+");
	if (!fp)
		return -1;

	fseek(fp, offset, SEEK_SET);

	fwrite(&day->elements, sizeof(int), 1, fp);
	for (int i = 0; i < day->elements; i++) {
		save_entry(fp, &day->entries[i]);
	}

	fclose(fp);
	return 0;
}

day_save_data_t *load_day_data_raw(long offset, const char *data_filename) {
	FILE *fp = fopen(data_filename, "rb");
	if (!fp) {
		printf("FP NULL");
		return NULL;
	}

	fseek(fp, offset, SEEK_SET);

	day_save_data_t *day = malloc(sizeof(day_save_data_t));
	fread(&day->elements, sizeof(int), 1, fp);

	day->entries = malloc(sizeof(calender_entry_t) * day->elements);
	for (int i = 0; i < day->elements; i++) {
		load_entry(fp, &day->entries[i]);
	}

	fclose(fp);
	return day;
}

day_save_data_t *loadDay(int day_num, int month, int year) {
	int key = create_save_key(day_num, month, year);

	int offset = get_day_data_offset(key, "saves/date.idx");
	if (offset == -1) {
		return NULL;
	}

	return load_day_data_raw(offset, "saves/date.dat");
}

void saveDay(day_save_data_t *day, int day_num, int month, int year) {
	int key = create_save_key(day_num, month, year);
	day_save_data_t *data = loadDay(day_num, month, year);

	if (data != NULL) {
		int original_count = data->elements;
		int new_count = original_count + day->elements;

		data->entries = realloc(data->entries, sizeof(calender_entry_t) * new_count);

		for (int j = 0; j < day->elements; j++) {
			data->entries[original_count + j] = day->entries[j];
		}

		data->elements = new_count;

		int data_day_offset = get_day_data_offset(key, "saves/date.idx");

		// if data of the day is last element, overwrite
		if (get_last_offset("saves/date.dat") == data_day_offset) {
			save_day_data_raw_at(data, "saves/date.dat", data_day_offset);
		} else {
			int offset = save_day_data_raw(data, "saves/date.dat");
			write_save_idx(key, (int)offset, "saves/date.idx");
		}

		free(data->entries);
		free(data);
	} else {
		// first entry for this day
		long offset = save_day_data_raw(day, "saves/date.dat");
		if (offset >= 0) {
			write_save_idx(key, (int)offset, "saves/date.idx");
		}
	}
}

void overwriteDayData(day_save_data_t *day, int day_num, int month, int year){
	int key = create_save_key(day_num, month, year);
	day_save_data_t *data = loadDay(day_num, month, year);

	if (data != NULL){
		if(!(data->elements < day->elements)){
			int offset = get_day_data_offset(key, "saves/date.idx");
			save_day_data_raw_at(day, "saves/date.dat", offset);
		}
	}
}

static int open_file_and_read_header(const char *filename, FILE **out_fp, file_header_t *out_header, int create_if_missing) {
    FILE *fp = fopen(filename, "r+b");
    if (!fp) {
        if (!create_if_missing) return -1;
        fp = fopen(filename, "w+b"); // create
        if (!fp) return -1;
        // initialize header
        memset(out_header, 0, sizeof(file_header_t));
        out_header->latest_rule_id = 0;
        out_header->last_offset = (int)sizeof(file_header_t);
        // write header
        if (fseek(fp, 0, SEEK_SET) != 0 || fwrite(out_header, sizeof(file_header_t), 1, fp) != 1) {
            fclose(fp);
            return -1;
        }
        fflush(fp);
        *out_fp = fp;
        return 0;
    }
    // read header
    if (fseek(fp, 0, SEEK_SET) != 0 || fread(out_header, sizeof(file_header_t), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    if (out_header->last_offset < (int)sizeof(file_header_t)) {
        out_header->last_offset = (int)sizeof(file_header_t);
    }
    *out_fp = fp;
    return 0;
}

/* --- Write a rule record to data file. returns offset or -1 on error --- */
int write_rule_data(rule_t *rule, const char *filename) {
    if (!rule || !filename) return -1;

    FILE *fp = NULL;
    file_header_t header;
    if (open_file_and_read_header(filename, &fp, &header, 1) != 0) return -1;

    // assign id if needed
    if (rule->rule_id == 0) {
        header.latest_rule_id++;
        rule->rule_id = header.latest_rule_id;
    }

    int offset = header.last_offset;

    if (fseek(fp, offset, SEEK_SET) != 0) { fclose(fp); return -1; }

    // write seven ints (rule_id, start_y/m/d, end_y/m/d)
    if (fwrite(&rule->rule_id, sizeof(int), 1, fp) != 1) goto fail;
    if (fwrite(&rule->start_year, sizeof(int), 1, fp) != 1) goto fail;
    if (fwrite(&rule->start_month, sizeof(int), 1, fp) != 1) goto fail;
    if (fwrite(&rule->start_day, sizeof(int), 1, fp) != 1) goto fail;
    if (fwrite(&rule->end_year, sizeof(int), 1, fp) != 1) goto fail;
    if (fwrite(&rule->end_month, sizeof(int), 1, fp) != 1) goto fail;
    if (fwrite(&rule->end_day, sizeof(int), 1, fp) != 1) goto fail;

    // entry type
    if (fwrite(&rule->type, sizeof(entry_type_t), 1, fp) != 1) goto fail;

    // string length and data
    unsigned char str_len = rule->data[0];
    if (fwrite(&str_len, sizeof(unsigned char), 1, fp) != 1) goto fail;
    if (str_len > 0) {
        if (fwrite(&rule->data[1], 1, str_len, fp) != str_len) goto fail;
    }

    // update header.last_offset
    int rule_size = (7 * sizeof(int)) + (int)sizeof(entry_type_t) + (int)sizeof(unsigned char) + (int)str_len;
    header.last_offset = offset + rule_size;

    // write header back
    if (fseek(fp, 0, SEEK_SET) != 0 || fwrite(&header, sizeof(file_header_t), 1, fp) != 1) goto fail;
    fflush(fp);
    fclose(fp);
    return offset;

fail:
    fclose(fp);
    return -1;
}

/* --- Write one index entry (append) and update its file header --- */
int write_rule_idx(int key, int offset, const char *filename) {
    if (!filename) return -1;

    FILE *fp = NULL;
    file_header_t header;
    if (open_file_and_read_header(filename, &fp, &header, 1) != 0) return -1;

    idx_entry_t entry;
    entry.key = key;
    entry.offset = offset;

    if (fseek(fp, header.last_offset, SEEK_SET) != 0) { fclose(fp); return -1; }
    if (fwrite(&entry, sizeof(idx_entry_t), 1, fp) != 1) { fclose(fp); return -1; }

    header.last_offset += sizeof(idx_entry_t);

    if (fseek(fp, 0, SEEK_SET) != 0 || fwrite(&header, sizeof(file_header_t), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    fflush(fp);
    fclose(fp);
    return 0;
}

/* --- Save rule: writes rule data and index entries for each month in the range --- */
void save_rule(rule_t *rule, const char *data_file, const char *idx_file) {
    if (!rule || !data_file || !idx_file) return;

    int offset = write_rule_data(rule, data_file);
    if (offset == -1) {
        fprintf(stderr, "write_rule_data failed\n");
        return;
    }

    int start_year = rule->start_year;
    int start_month = rule->start_month;
    int end_year = rule->end_year;
    int end_month = rule->end_month;

    int current_year = start_year;
    int current_month = start_month;

    while (current_year < end_year || (current_year == end_year && current_month <= end_month)) {
        int key = current_year * 100 + current_month;
        if (write_rule_idx(key, offset, idx_file) != 0) {
            fprintf(stderr, "write_rule_idx failed for key %d\n", key);
        }

        current_month++;
        if (current_month > 12) {
            current_month = 1;
            current_year++;
        }
    }
}

/* --- Load rules for a year/month and filter by day number --- */
/* returns dynamically allocated array of rule_t; count is set; caller must free() the returned pointer */
rule_t *load_rules_for_day(int day_num, int month, int year, int *out_count, const char *idx_file, const char *data_file) {
    if (!out_count) return NULL;
    *out_count = 0;
    if (!idx_file || !data_file) return NULL;

    FILE *idx_fp = fopen(idx_file, "rb");
    if (!idx_fp) {
        return NULL;
    }

    file_header_t idx_header;
    if (fread(&idx_header, sizeof(file_header_t), 1, idx_fp) != 1) {
        fclose(idx_fp);
        return NULL;
    }

    int key = year * 100 + month;

    // collect offsets (linear scan - safe even if idx is not sorted)
    int offsets_capacity = INITIAL_OFFSETS_CAP;
    int *offsets = malloc(sizeof(int) * offsets_capacity);
    if (!offsets) { fclose(idx_fp); return NULL; }
    int offsets_count = 0;

    if (fseek(idx_fp, sizeof(file_header_t), SEEK_SET) != 0) { fclose(idx_fp); free(offsets); return NULL; }

    long read_pos = ftell(idx_fp);
    while (read_pos + (long)sizeof(idx_entry_t) <= (long)idx_header.last_offset) {
        idx_entry_t entry;
        if (fread(&entry, sizeof(idx_entry_t), 1, idx_fp) != 1) break;
        if (entry.key == key) {
            if (offsets_count >= offsets_capacity) {
                offsets_capacity *= 2;
                int *tmp = realloc(offsets, sizeof(int) * offsets_capacity);
                if (!tmp) { free(offsets); fclose(idx_fp); return NULL; }
                offsets = tmp;
            }
            offsets[offsets_count++] = entry.offset;
        }
        read_pos = ftell(idx_fp);
    }
    fclose(idx_fp);

    if (offsets_count == 0) {
        free(offsets);
        return NULL;
    }

    // open data file and read each rule, filter by day
    FILE *data_fp = fopen(data_file, "rb");
    if (!data_fp) { free(offsets); return NULL; }

    rule_t *rules = malloc(sizeof(rule_t) * offsets_count); // maximum possible
    if (!rules) { fclose(data_fp); free(offsets); return NULL; }
    int rules_count = 0;

    for (int i = 0; i < offsets_count; ++i) {
        long off = offsets[i];
        if (fseek(data_fp, off, SEEK_SET) != 0) continue;

        rule_t tmp;
        memset(&tmp, 0, sizeof(tmp));

        // read ints and type
        if (fread(&tmp.rule_id, sizeof(int), 1, data_fp) != 1) continue;
        if (fread(&tmp.start_year, sizeof(int), 1, data_fp) != 1) continue;
        if (fread(&tmp.start_month, sizeof(int), 1, data_fp) != 1) continue;
        if (fread(&tmp.start_day, sizeof(int), 1, data_fp) != 1) continue;
        if (fread(&tmp.end_year, sizeof(int), 1, data_fp) != 1) continue;
        if (fread(&tmp.end_month, sizeof(int), 1, data_fp) != 1) continue;
        if (fread(&tmp.end_day, sizeof(int), 1, data_fp) != 1) continue;
        if (fread(&tmp.type, sizeof(entry_type_t), 1, data_fp) != 1) continue;

        // read string length
        unsigned char str_len = 0;
        if (fread(&str_len, sizeof(unsigned char), 1, data_fp) != 1) continue;
        tmp.data[0] = str_len;

        if (str_len > 0) {
            size_t got = fread(&tmp.data[1], 1, (size_t)str_len, data_fp);
            if (got != str_len) continue;
        }
        tmp.data[str_len + 1] = '\0';

        // now test if the given day_num falls within this rule date range
        int day_value   = year * 10000 + month * 100 + day_num;
        int start_value = tmp.start_year * 10000 + tmp.start_month * 100 + tmp.start_day;
        int end_value   = tmp.end_year * 10000 + tmp.end_month * 100 + tmp.end_day;

        if (day_value >= start_value && day_value <= end_value) {
            // copy tmp into dynamic array
            memcpy(&rules[rules_count++], &tmp, sizeof(rule_t));
        }
    }

    fclose(data_fp);
    free(offsets);

    if (rules_count == 0) {
        free(rules);
        return NULL;
    }

    // shrink to exact size
    rule_t *shrink = realloc(rules, sizeof(rule_t) * rules_count);
    if (shrink) rules = shrink;

    *out_count = rules_count;
    return rules;
}

/* --- Merge rules into day structure --- */
day_save_data_t *loadDayWithRules(int day_num, int month, int year) {
    day_save_data_t *day_data = loadDay(day_num, month, year);

    int rule_count = 0;
    rule_t *rules = load_rules_for_day(day_num, month, year, &rule_count, "saves/rules.idx", "saves/rules.dat");

    if (rule_count == 0 || rules == NULL) {
        if (rules) free(rules);
        return day_data;
    }

    // Convert rules to calendar entries
    calender_entry_t *rule_entries = malloc(sizeof(calender_entry_t) * rule_count);
    if (!rule_entries) { free(rules); return day_data; }

    for (int i = 0; i < rule_count; ++i) {
        memset(&rule_entries[i], 0, sizeof(calender_entry_t));
        rule_entries[i].type = rules[i].type;
        int len = (int)rules[i].data[0];
        if (len > (int)sizeof(rule_entries[i].text) - 1) len = (int)sizeof(rule_entries[i].text) - 1;
        if (len > 0) memcpy(rule_entries[i].text, &rules[i].data[1], len);
        rule_entries[i].text[len] = '\0';
        rule_entries[i].ruleID = rules[i].rule_id;
    }

    // Merge into day_data
    if (day_data == NULL) {
        // No existing day data: take all rules
        day_data = malloc(sizeof(day_save_data_t));
        if (!day_data) { free(rule_entries); free(rules); return NULL; }
        day_data->elements = rule_count;
        day_data->entries = rule_entries;
    } else {
        // Existing day data: only add rules whose ruleID is not already present
        int original_count = day_data->elements;
        int new_entries = 0;

        // First, count how many new rules are actually needed
        for (int i = 0; i < rule_count; ++i) {
            int exists = 0;
            for (int j = 0; j < original_count; ++j) {
                if (day_data->entries[j].ruleID == rule_entries[i].ruleID) {
                    exists = 1;
                    break;
                }
            }
            if (!exists) new_entries++;
        }

        if (new_entries > 0) {
            // Reallocate day_data->entries to fit the new rules
            calender_entry_t *tmp = realloc(day_data->entries, sizeof(calender_entry_t) * (original_count + new_entries));
            if (!tmp) {
                free(rule_entries);
                free(rules);
                return day_data; // can't reallocate, just return existing day_data
            }
            day_data->entries = tmp;

            // Copy only new rules
            int idx = original_count;
            for (int i = 0; i < rule_count; ++i) {
                int exists = 0;
                for (int j = 0; j < original_count; ++j) {
                    if (day_data->entries[j].ruleID == rule_entries[i].ruleID) {
                        exists = 1;
                        break;
                    }
                }
                if (!exists) {
                    day_data->entries[idx++] = rule_entries[i];
                }
            }
            day_data->elements = original_count + new_entries;
        }

        free(rule_entries);
    }

    free(rules);
    return day_data;
}
/* --- End of corrected implementation --- */