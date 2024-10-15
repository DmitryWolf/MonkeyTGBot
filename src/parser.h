#ifndef PARSER_H
#define PARSER_H

// #include <stddef.h> // "jansson.h" contains "stddef.h"
#include <jansson.h>
#define MAX_TEXT_SIZE 8196 // 8192+4

typedef struct {
    long long message_id_;
    long long from_id_;
    char first_name_[100];
    char username_[100];
    long long chat_id_;
    char chat_title_[100];
    char text_[MAX_TEXT_SIZE];
} TelegramMessage;

TelegramMessage* parse_telegram_response(const char *response, size_t *size);

int get_last_update_id(const char *response, int is_first_parsing);

#endif // PARSER_H