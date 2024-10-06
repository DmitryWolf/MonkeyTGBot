#include "monkey.h"

#include <stdlib.h>
#include <time.h>

void init_rand() {
    srand(time(NULL));
}

// Generate random string from arr
char* generate_random_monkey_string(char lines[][MAX_LINE_LEN], int total_lines) {
    if (total_lines == 0) {
        return NULL;
    }
    int random_index = rand() % total_lines;
    return lines[random_index];
}

int myrandom(int begin, int end) {
    return begin + rand() % (end - begin + 1);
}