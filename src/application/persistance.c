#include "persistance.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *save_dir = "saves/";
char *files_names[4] = {"date.idx", "date.dat", "rules.idx", "rules.dat"};

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

void create_save_file(char *filename) {
	FILE *fp = fopen(filename, "wb");

	file_header_t header = {.magic = MAGIC,
	                        .version = (version_t){
	                            .major = MAJOR_VERSION,
	                            .minor = MINOR_VERSION,
	                            .counter = COUNT_VERSION,
	                        }};

	if (fp != NULL) {
		fwrite(&header, sizeof(file_header_t), 1, fp);
		fclose(fp);
	}
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
	fwrite(day->entries, sizeof(calender_entry_t), day->elements, fp);

	fclose(fp);
	return offset;
}

int save_day_data_raw_at(day_save_data_t *day, const char *data_filename, int offset) {
	FILE *fp = fopen(data_filename, "rb+");
	if (!fp)
		return -1;

	fseek(fp, offset, SEEK_SET);

	fwrite(&day->elements, sizeof(int), 1, fp);
	fwrite(day->entries, sizeof(calender_entry_t), day->elements, fp);

	fclose(fp);
	return 0;
}

day_save_data_t *load_day_data_raw(long offset, const char *data_filename) {
	FILE *fp = fopen(data_filename, "rb");
	if (!fp){
		printf("FP NULL");
		return NULL; 
	}
		

	fseek(fp, offset, SEEK_SET);

	day_save_data_t *day = malloc(sizeof(day_save_data_t));
	fread(&day->elements, sizeof(int), 1, fp);

	day->entries = malloc(sizeof(calender_entry_t) * day->elements);
	fread(day->entries, sizeof(calender_entry_t), day->elements, fp);

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

		// grow entries array
		data->entries = realloc(data->entries, sizeof(calender_entry_t) * new_count);

		// append new entries
		for (int j = 0; j < day->elements; j++) {
			data->entries[original_count + j] = day->entries[j];
		}

		data->elements = new_count;

		// append merged block and update index
		long offset = save_day_data_raw(data, "saves/date.dat");
		if (offset >= 0) {
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
