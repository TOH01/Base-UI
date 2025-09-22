#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "persistance.h"

char * save_dir = "saves/";
char * files_names[4] = {"date.idx", "date.dat", "rules.idx", "rules.dat"};

bool dir_exists(const char *path) {
    DWORD attribs = GetFileAttributesA(path);
    return (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
}

void make_dir(const char *path) {
    CreateDirectoryA(path, NULL);
}

bool file_exists(char * filename){
    FILE *fp = fopen(filename, "r");
    if (fp) {
        fclose(fp);
        return true;
    }
    return false; 
}

void create_save_file(char * filename){
    FILE *fp = fopen(filename, "w");

    file_header_t header = {
        .magic = MAGIC,
        .version = (version_t) {
            .major = MAJOR_VERSION,
            .minor = MINOR_VERSION,
            .counter = COUNT_VERSION,
        }
    };

    if(fp != NULL){
        fwrite(&header, sizeof(file_header_t), 1, fp);
        fclose(fp);
    }

}

void create_file_system(void){
    
    if (!dir_exists(save_dir)) {
        make_dir(save_dir);
    }

    for (int i = 0; i < (int)(sizeof(files_names) / sizeof(files_names[0])); i++)
    {
        
        char * file_path = malloc(strlen(save_dir) + strlen(files_names[i]));
        sprintf(file_path, "%s%s", save_dir, files_names[i]);
        
        if(!file_exists(file_path)){
            create_save_file(file_path);
        }
    }
    
}