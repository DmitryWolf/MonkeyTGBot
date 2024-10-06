#include "tools.h"
#include <stdlib.h>
#include <string.h>

int read_line_from_file(char *info, size_t size, const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return -1;
    }

    if (fgets(info, size, file) == NULL) {
        perror("Error reading from file");
        fclose(file);
        return -1;
    }

    size_t len = strlen(info);
    if (len > 0 && info[len - 1] == '\n') {
        info[len - 1] = '\0';
    }

    fclose(file);
    return 0;
}

int read_lines(const char *filename, char lines[][MAX_LINE_LEN], int max_lines) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return -1;
    }

    int count = 0;
    while (count < max_lines && fgets(lines[count], MAX_LINE_LEN, file) != NULL) {
        // Delete '\n' if exists
        size_t len = strlen(lines[count]);
        if (len > 0 && lines[count][len - 1] == '\n') {
            lines[count][len - 1] = '\0';
        }
        count++;
    }

    fclose(file);
    return count; // count of lines
}

int* get_len_of_symbols(const char *json) {
    int len = strlen(json);
    int *result = malloc(len * sizeof(int));
    if (!result) {
        fprintf(stderr, "Error in malloc in get_len_of_symbols\n");
        return NULL;
    }

    for (int i = 0; i < len; ++i) {
        int q = json[i];
        if (q >= 0 && q <= 127) {
            result[i] = 1;
        } else {
            result[i] = 2;
            i++;
            result[i] = 0;
        }
    }

    return result;
}