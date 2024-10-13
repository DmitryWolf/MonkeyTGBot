#ifndef TASK_H
#define TASK_H
#include "monkeybot.h"

typedef struct {
    Telebot *bot_;
    const char *message_;
    // TelegramMessage tm_;
    long long message_id_;
    long long from_id_;
    long long chat_id_;
    connection* context_;
    MessageType type_;
} Task;

#endif // TASK_H