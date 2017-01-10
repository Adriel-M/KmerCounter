#include <stdio.h>

#include "parsefile.h"
#include "hash.h"

/* Given an array of filepaths, open each file and generate the substring count */
void parse_files(char **list_of_files, int num_of_files, SUBSTRING_COUNTER_TYPE **storage) {

    SUBSTRING_COUNTER_TYPE *storage_array = *storage;

    /* Iterating through all the files that we are given. List of files will always be different for each child. */
    for (int i = 0; i < num_of_files; i++) {

        FILE *current_file;
        current_file = fopen(list_of_files[i], "rb");

        if (current_file == NULL) {
            perror("ERROR: fopen\n");
        } else {

            /* Initialize the offset value to 0. We need this value
            for fseek. */
            int offset = 0;

            /* Read till the end of file. */
            while (!feof(current_file)) {
                char buffer[5];

                /* If it fails to read the 5 character substring successfully it will return a 0,
                breaking out of the loop. */
                if (fread(&buffer, sizeof(char), 5, current_file) == 0) {
                    break;
                }

                /* If buffer is null we go ahead and hash the values in the buffer array. */
                if (buffer != NULL) {

                    /* Calculate the hash value (base 4 to base 10). */
                    int hash_temp = hash(buffer, 5);

                    /* The hash function will return a -1 if the substring contains invalid characters.
                    So if it doesn't equate to that we increment it's index on the array by one. */
                    if (hash_temp != -1) {
                        storage_array[hash_temp] += 1;
                    }
                }

                /* Increment the offset to move along in the binary file. */
                offset++;

                /* fseek using offset to count how many positions from the start of the file to position
                itself. */
                fseek(current_file, offset, SEEK_SET);
            }
        }

        /* Close the opening of the file, to prevent memory leaks, and of course good programming
        practice. */
        fclose(current_file);
    }
}