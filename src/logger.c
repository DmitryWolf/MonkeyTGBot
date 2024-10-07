#include "logger.h"

#include <string.h>
#include <time.h>

pthread_mutex_t mutex_log_ = PTHREAD_MUTEX_INITIALIZER;

void LOG_INIT() {
    pthread_mutex_init(&mutex_log_, NULL);
}

void LOG(const char *path, const char *text, int is_JSON_response) {
    const char *body;
    if (is_JSON_response == 1) {
        body = strstr(text, "\r\n\r\n");
        if (!body) {
            printf("Error: The response body was not found\n");
            body = text;
        } else {
            // Skip "\r\n\r\n"
            body += 4;
        }
    } else {
        body = text;
    }
    size_t size = strlen(body);

    pthread_mutex_lock(&mutex_log_);


    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char time_buffer[20];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);


    FILE *file = fopen(path, "a");
    if (!file) {
        fprintf(stderr, "Error in opening log-file\n");
        return;
    }
    fprintf(file, "DUMP [%s] : ", time_buffer);
    fwrite(body, 1, size, file);
    fprintf(file, "\n");
    fclose(file);


    pthread_mutex_unlock(&mutex_log_);
    
}

void LOG_DESTROY() {
    pthread_mutex_destroy(&mutex_log_);
}