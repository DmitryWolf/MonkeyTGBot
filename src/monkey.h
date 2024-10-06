#ifndef MONKEY_H
#define MONKEY_H

#include "tools.h"

#define MAX_MONKEY_LINES 100

void init_rand();

// Get random line from file
char* generate_random_monkey_string(char lines[][MAX_LINE_LEN], int total_lines);

// just random function
int myrandom(int begin, int end);


#endif // MONKEY_H