#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "FileListGenerator.h"
#include "parsefile.h"


void print_results(FILE *output, SUBSTRING_COUNTER_TYPE *total_count);

int main(int argc, char **argv)
{
    // the following argument
    int ch;

    int directory_set = 0;
    int output_file_set = 0;

    // variables we need to set
    char folder_location[1024];
    char output_file[1024];

    //* Default value if unspecified */
    int num_process = 1;

    /* Get arguments from optional parameters */
    while ((ch = getopt(argc, argv, "d:n:o:")) != -1) {
        switch (ch) {
            case 'd':
                strcpy(folder_location, optarg);
                directory_set = 1;
                break;
            case 'n':
                num_process = (int) strtol(optarg, NULL, 10);
                if (num_process <= 0) {
                    fprintf(stderr, "ERROR: invalid number specified for number of processes.\n");
                    exit(1);
                }
                break;
            case 'o':
                strcpy(output_file, optarg);
                output_file_set = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s -d <path to directory> [-n] <number of children> -o <name of output file>\n", argv[0]);
                exit(1);
        }
    }

    if (!directory_set) {
        fprintf(stderr, "ERROR: -d <path to directory> must be specified.\n");
        exit(1);
    }

    /*
     * Generate file lists from directory and check if we can access it first
     * Set num_process to be equal to number of files if num_process > num of files
     */
    FILE *output_fp;

    if (output_file_set) {
        output_fp = fopen(output_file, "w");
        if (output_fp == NULL) {
            perror("ERROR: creating output file");
            exit(1);
        }
    } else {
        output_fp = stdout;
    }

    int number_of_files = count_files(folder_location);

    if (number_of_files <= 0) {
        fprintf(stderr, "no files\n");
        return 0;
    }

    /* If num of process specified is greater than number of files, set num of process to be num of files */
    if (num_process > number_of_files) {
        num_process = number_of_files;
    }

    /* Decide how to evenly distribute the files per children */
    Files_nn **file_array = retrieve_filenames(folder_location, num_process, number_of_files);

    if (file_array == NULL) {
        fprintf(stderr, "Problem splitting off the files\n");
        exit(1);
    }


    /* The array that will keep track of the substrings */
    SUBSTRING_COUNTER_TYPE *total_count = calloc(1024, sizeof(SUBSTRING_COUNTER_TYPE));


    /* Start timer */
    struct timeval start_time, end_time;
    double time_diff;

    if (gettimeofday(&start_time, NULL) == -1) {
        perror("gettimeofday\n");
        exit(1);
    }

    /* Single Process */
    if (num_process == 1) {
        parse_files(file_array[0]->filenames, file_array[0]->number_of_files, &total_count);
    } else { /* Multi Process */
        int fd[num_process][2];
        for (int i = 0; i < num_process; i++) {
            if (pipe(fd[i]) == -1) {
                perror("ERROR: Initializing pipe\n");
                exit(1);
            }
            int n = fork();

            if (n == 0) {
                /* No need to read from the pipe */
                if (close(fd[i][0]) == -1) {
                    perror("ERROR: Closing pipe\n");
                }
                SUBSTRING_COUNTER_TYPE *child_count = calloc(1024, sizeof(SUBSTRING_COUNTER_TYPE));
                parse_files(file_array[i]->filenames, file_array[i]->number_of_files, &child_count);
                for (int l = 0; l < 1024; l++) {
                    SUBSTRING_COUNTER_TYPE value = child_count[l];
                    if (write(fd[i][1], &value, sizeof(SUBSTRING_COUNTER_TYPE)) != sizeof(SUBSTRING_COUNTER_TYPE)) {
                        perror("ERROR: Writing to pipe\n");
                    }
                }
                /* Finished writing to pipe */
                if (close(fd[i][1]) == -1) {
                    perror("ERROR: Closing pipe");
                }

                /* Everything freed from parent must be freed from child as well */
                fclose(output_fp);
                free(child_count);
                free(total_count);
                clear_filenames(file_array, num_process);
                exit(0);
            } else if (n > 0) {
                if (close(fd[i][1]) == -1) {
                    perror("ERROR: Closing pipe\n");
                }
            } else {
                perror("ERROR: Fork\n");
                exit(1);
            }
        }

        /* Wait for sequential children to finish then check their exit and signal status.
         * If a child terminated fine, retrieve the piped contents. */
        for (int i = 0; i < num_process; i++) {
            pid_t pid;
            int status;

            if ((pid = wait(&status)) == -1) {
                perror("wait");
            } else {
                if (WIFEXITED(status)) {
                    int e_status = WEXITSTATUS(status);
                    if (e_status == 0) {
                        for (int j = 0; j < 1024; j++) {
                            SUBSTRING_COUNTER_TYPE value = 0;
                            if (read(fd[i][0], &value, sizeof(SUBSTRING_COUNTER_TYPE)) != sizeof(SUBSTRING_COUNTER_TYPE)) {
                                perror("Read\n");
                            }
                            total_count[j] += value;
                        }
                    } else {
                        fprintf(stderr, "Child %d terminated with %d\n", pid, e_status);
                    }
                } else if (WIFSIGNALED(status)) {
                    int e_signal = WTERMSIG(status);
                    fprintf(stderr, "Child %d signalled with %d\n", pid, e_signal);
                }
            }

        }
    }
    /* Parent only reaches this */

    /* End timer */
    if (gettimeofday(&end_time, NULL) == -1) {
        perror("gettimeofday\n");
        exit(1);
    }
    time_diff = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    fprintf(stderr, "%.4f\n", time_diff);

    print_results(output_fp, total_count);
    fclose(output_fp);
    free(total_count);
    clear_filenames(file_array, num_process);
    return 0;
}

/*
 * Iterate through every single permutation of substrings in alphabetical order.
 * total_count is already in alphabetical order
 */
void print_results(FILE *output, SUBSTRING_COUNTER_TYPE *total_count) {
    char bases[4] = {'a', 'c', 'g', 't'};
    int pos = 0;
    for (int z = 0; z < 4; z++) {
        for (int x = 0; x < 4; x++) {
            for (int c = 0; c < 4; c++) {
                for (int v = 0; v < 4; v++) {
                    for (int b = 0; b < 4; b++) {
                        fprintf(output, "%c%c%c%c%c: %d\n",
                                bases[z], bases[x], bases[c], bases[v], bases[b], total_count[pos]);
                        pos++;
                    }
                }
            }
        }
    }
}
