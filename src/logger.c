#include "logger.h"

#include <string.h>
#include <time.h>

pthread_mutex_t mutex_log_ = PTHREAD_MUTEX_INITIALIZER;

void LOG_INIT() {
    pthread_mutex_init(&mutex_log_, NULL);
}

void LOG(const char *path, const char *text, int isJSONResponse) {
    const char *body;
    if (isJSONResponse == 1) {
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

    

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    pthread_mutex_lock(&mutex_log_);

    FILE *file = fopen(path, "a");
    if (!file) {
        fprintf(stderr, "Error in opening log-file\n");
        return;
    }
    fprintf(file, "DUMP [%s] : ", timeBuffer);
    fwrite(body, 1, size, file);
    fprintf(file, "\n");
    fclose(file);

    pthread_mutex_unlock(&mutex_log_);
    
}

void LOG_DESTROY() {
    pthread_mutex_destroy(&mutex_log_);
}