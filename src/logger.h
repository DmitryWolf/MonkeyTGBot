#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#define REQUEST_PATH "resources/logs/requests.txt"
#define RESPONSE_PATH "resources/logs/responses.txt"

void LOG(const char *path, const char *text, int isJSONResponse);

#endif // LOGGER_H