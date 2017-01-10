#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FileListGenerator.h"

/* Generate an array of Files_nn structs that hold an array of filepaths and the number of files stored */
Files_nn ** retrieve_filenames(const char *folder_set, int number_of_children, int number_of_files) {
    int path_length = (int) strlen(folder_set);
    int added = 0;

    /* check to see if the path provided contains '/' */
    if (folder_set[path_length - 1] != '/') {
        path_length++;
        added = 1;
    }

    /* create new array with the folder_path. Necessary just in case the provided didn't contain '/' */
    char folder_path[path_length + 1];;
    strcpy(folder_path, folder_set);
    if (added) {
        strcat(folder_path, "/");
    }

    if (number_of_children <= 0) {
        return NULL;
    }

    struct dirent *dir;

    /* Create an array of file names */
    DIR *d_names;
    d_names = opendir(folder_path);
    char **file_names = malloc(sizeof(char *) * number_of_files);
    if (d_names) {
        int pos = 0;
        while ((dir = readdir(d_names)) != NULL) {
            char *d_name = dir->d_name;
            /* ignore hidden files */
            if (d_name[0] != '.') {
                int name_length = (int) strlen(d_name);
                file_names[pos] = malloc(sizeof(char) * (name_length + 1 + path_length + 1));
                strcpy(file_names[pos], folder_path);
                strcat(file_names[pos], d_name);
                pos++;
            }
        }
        closedir(d_names);
    } else {
        perror("ERROR: Failed to access directory.\n");
        exit(1);
    }

    // We need to find out how to evenly distribute the load using the number of children. DONE.
    int even_load = number_of_files / number_of_children;
    int remainder = number_of_files % number_of_children;

    Files_nn **files_nn_array = malloc(sizeof(Files_nn *) * number_of_children);


    /* malloc the Files struct */
    for (int i = 0; i < number_of_children; i++) {
        Files_nn *temp = malloc(sizeof(Files_nn));
        temp->number_of_files = even_load;
        files_nn_array[i] = temp;
    }

    /* for the remainder files, add them to the files in front */
    for (int i = 0; i < remainder; i++) {
        Files_nn *temp = files_nn_array[i];
        temp->number_of_files++;
    }

    /* malloc the string array now */
    for (int i = 0; i < number_of_children; i++) {
        Files_nn *temp = files_nn_array[i];
        temp->filenames = malloc(sizeof(char *) * temp->number_of_files);
    }

    /* now add strings to the array inside the structs */
    int added_files = 0;
    for (int i = 0; i < number_of_children; i++) {
        Files_nn *temp = files_nn_array[i];
        for (int j = 0; j < temp->number_of_files; j++) {
            int full_path =  (int)  strlen(file_names[added_files]);
            char *filename = malloc(sizeof(char) * (full_path + 1));
            strcpy(filename, file_names[added_files]);
            temp->filenames[j] = filename;
            added_files++;
        }
    }

    for (int i = 0; i < number_of_files; i++) {
        free(file_names[i]);
    }
    free(file_names);

    return files_nn_array;
}

/* Clear everything from the files_nn array */
void clear_filenames(Files_nn **files_nn_array, int number_of_children) {
    for (int i = 0; i < number_of_children; i++) {
        Files_nn *temp = files_nn_array[i];
        for (int j = 0; j < temp->number_of_files; j++) {
            free(temp->filenames[j]);
        }
        free(temp->filenames);
        free(temp);
    }
    free(files_nn_array);
}

/* Count the number of files in a folder path */
int count_files(const char *folder_path) {

    DIR *d_counting;
    d_counting = opendir(folder_path);
    int number_of_files = 0;

    struct dirent *dir;

    /* Count the number of files. */
    if (d_counting) {
        while ((dir = readdir(d_counting)) != NULL) {
            /* Ignore hidden files */
            char *d_name = dir->d_name;
            if (d_name[0] != '.') {
                number_of_files++;
            }
        }
        closedir(d_counting);
    } else {
        perror("ERROR: Failed to access directory.\n");
        exit(1);
    }

    return number_of_files;
}
