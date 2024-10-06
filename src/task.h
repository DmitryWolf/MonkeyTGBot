#ifndef TASK_H
#define TASK_H
#include "monkeybot.h"

typedef struct {
    Telebot *bot;
    const char *message;
    TelegramMessage *tm;
    connection* context;
    MessageType type;
} Task;

#endif // TASK_H