#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
// #define printf if (0) printf

#define MAX_LINE_LEN 256

// Read information from a file
int read_line_from_file(char *info, size_t size, const char *path);

// Read lines from file
int read_lines(const char *filename, char lines[][MAX_LINE_LEN], int max_lines);

// "Decode" to UTF-8
int* get_len_of_symbols(const char *json);


#endif // TOOLS_H