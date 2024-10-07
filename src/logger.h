#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <pthread.h>

#define REQUEST_PATH "resources/logs/requests.txt"
#define RESPONSE_PATH "resources/logs/responses.txt"


extern pthread_mutex_t mutex_log_;

void LOG_INIT();

void LOG(const char *path, const char *text, int is_JSON_response);

void LOG_DESTROY();

#endif // LOGGER_H