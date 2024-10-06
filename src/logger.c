#include "logger.h"

#include <string.h>
#include <time.h>

void LOG(const char *path, const char *text, int isJSONResponse) {
    const char *body;
    if (isJSONResponse == 1) {
        body = strstr(text, "\r\n\r\n");
        if (!body) {
            printf("Error: The response body was not found\n");
        }
        // Skip "\r\n\r\n"
        body += 4;
    } else {
        body = text;
    }

    size_t size = strlen(body);

    FILE *file = fopen(path, "a");
    if (!file) {
        fprintf(stderr, "Не удалось открыть лог-файл.\n");
        return;
    }

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    fprintf(file, "DUMP [%s] : ", timeBuffer);

    fwrite(body, 1, size, file);

    fprintf(file, "\n");

    fclose(file);
}