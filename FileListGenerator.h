#ifndef A3_FILELISTGENERATOR_H
#define A3_FILELISTGENERATOR_H

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct file_name_num {
    char** filenames;
    int number_of_files;
} Files_nn;

Files_nn ** retrieve_filenames(const char* folder_path, int number_of_children, int number_of_files);

int count_files(const char* folder_path);

void clear_filenames(Files_nn **file_nn_array, int number_of_files);

#endif //A3_FILELISTGENERATOR_H
