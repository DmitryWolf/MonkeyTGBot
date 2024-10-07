#ifndef TELEBOT_H
#define TELEBOT_H

#include "parser.h"
#include "banword.h"
#include "monkey.h"
#include "connection.h" // "threadpoo.h" contains "connection.h"
#include "threadpool.h"

#define PATH_TO_TOKEN "resources/token.txt"
#define PATH_TO_BANWORDS "resources/banwords.txt"
#define PATH_TO_TEXT_ANSWERS "resources/monkeyanswers.txt"
#define PATH_TO_STICKERS_ANSWERS "resources/monkeystickers.txt"
#define DEFAULT_MESSAGE ""
#define DEFAULT_SIZE 1024 
#define REQUEST_SIZE 2048
#define RESPONSE_SIZE 32768
#define LIMIT_REQUESTS 100
#define SSLREAD_SIZE 16384 // max size of 1 SSL_read response
#define TIME_TO_SLEEP 60

typedef enum{
    TEXT = 0,
    REPLY_TEXT,
    REPLY_STICKER
} MessageType;

typedef struct {
    char token_[128];
    const char *host_;
    const char *port_;
    connection context_;
    int offset_;

    Banmonkey banwords_[MAX_BANWORDS];
    int banword_count_;

    char monkey_answers_[MAX_MONKEY_LINES][MAX_LINE_LEN];
    int count_monkey_answers_;

    char monkey_stickers_[MAX_MONKEY_LINES][MAX_LINE_LEN];
    int count_monkey_stickers_;

    threadpool pool_;
} Telebot;

// Send message (using in threadpool)
void send_msg(void* args);

// Initialize Telebot structure
int telebot_init(Telebot *bot, const char *token_path);

// Free resources used by Telebot
void telebot_destroy(Telebot *bot);

// Get updates from Telegram
int telebot_get_updates(Telebot *bot, char *response, size_t response_size);

// Process incoming messages
int telebot_process_updates(Telebot *bot, const char *response);

// Generate update request
void make_update_request(Telebot *bot, char* request);

// Generate text-message to chat
void make_text_message(
    Telebot *bot, const char *escaped_message, 
     char *request, long long chat_id);

// Generate text-message with reply
void make_text_message_with_reply(
    Telebot *bot, const char *escaped_message, char *request, 
    long long chat_id, long long reply_to_message_id);

// Generate sticker-message with reply
void make_sticker_message_with_reply(
    Telebot *bot, const char *id_sticker, char* request,
    long long chat_id, long long reply_to_message_id);

// Send HTTPS request
int send_https_request(connection* context, const char *request, char *response, size_t response_size);

// Add banwords
void add_banwords_to_array(Telebot *bot);

// Prepare request for URL-format
void normalize_url_request(const char *message, char *escaped_message, size_t escaped_size);

#endif // TELEBOT_H