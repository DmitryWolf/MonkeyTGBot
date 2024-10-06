#ifndef BANWORD_H
#define BANWORD_H

#define MAX_BANWORDS 100

typedef struct {
    char *word;
    unsigned long hash;
} Banmonkey;


// Add banword to array
void add_banword(Banmonkey banwords[], int *banword_count, const char *word);

// Find banwords in text
int* find_banwords(Banmonkey banwords[], int banword_count, const char *text, int *result_count);


#endif // BANWORD_H