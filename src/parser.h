#ifndef PARSER_H
#define PARSER_H

// #include <stddef.h> // "jansson.h" contains "stddef.h"
#include <jansson.h>

typedef struct {
    long long message_id;
    long long from_id;
    char first_name[100];
    char username[100];
    long long chat_id;
    char chat_title[100];
    char text[1024];
} TelegramMessage;

TelegramMessage* parse_telegram_response(const char *response, size_t *size);

#endif // PARSER_H