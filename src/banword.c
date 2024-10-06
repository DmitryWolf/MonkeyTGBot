#include "banword.h"
#include "hash.h"

#include <string.h>
#include <stdlib.h>


void add_banword(Banmonkey banwords[], int *banword_count, const char *word) {
    if (*banword_count < MAX_BANWORDS) {
        banwords[*banword_count].word = strdup(word);
        banwords[*banword_count].hash = hash(word);
        (*banword_count)++;
    }
}

int* find_banwords(Banmonkey banwords[], int banword_count, const char *text, int *result_count) {
    int text_len = strlen(text);
    int *results = malloc(text_len * sizeof(int));
    *result_count = 0;

    for (int i = 0; i < text_len; ++i) {
        for (int j = 0; j < banword_count; ++j) {
            int word_len = strlen(banwords[j].word);
            if (i + word_len <= text_len) {
                char substr[word_len + 1];
                strncpy(substr, text + i, word_len);
                substr[word_len] = '\0';

                unsigned long substr_hash = hash(substr);

                if (substr_hash == banwords[j].hash) {
                    // because of collisions
                    if (strncmp(substr, banwords[j].word, word_len) == 0) {
                        results[*result_count] = i;
                        results[*result_count + 1] = i + word_len;
                        *result_count += 2;
                    }
                }
            }
        }
    }
    return results;
}