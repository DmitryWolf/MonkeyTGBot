#ifndef PARSER_H
#define PARSER_H

// #include <stddef.h> // "jansson.h" contains "stddef.h"
#include <jansson.h>

typedef struct {
    long long message_id_;
    long long from_id_;
    char first_name_[100];
    char username_[100];
    long long chat_id_;
    char chat_title_[100];
    char text_[1024];
} TelegramMessage;

TelegramMessage* parse_telegram_response(const char *response, size_t *size);

#endif // PARSER_H