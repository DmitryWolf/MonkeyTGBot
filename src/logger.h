#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <pthread.h>

#define REQUEST_PATH "resources/logs/requests.txt"
#define RESPONSE_PATH "resources/logs/responses.txt"
#define ERROR_PATH "resources/logs/errors.txt"

extern pthread_mutex_t mutex_log_;

typedef enum{
    REQUEST,
    RESPONSE,
    ERROR
} LOG_TYPE;

void LOG_INIT();

char* LOG_DUMP();

void LOG(const char *path, const char *text, LOG_TYPE type);

void LOG_DESTROY();

#endif // LOGGER_H