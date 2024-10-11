#include "monkeybot.h"
#include "task.h"
#include "hash.h"

#include <unistd.h>
#include <netdb.h>
#include <openssl/err.h>


void send_msg(void* args) {
    Task* task = (Task*)(args);

    char request[REQUEST_SIZE];
    char response[RESPONSE_SIZE];

    char escaped_message[DEFAULT_SIZE];
    normalize_url_request(task->message_, escaped_message, sizeof(escaped_message));

    switch(task->type_) {
        case TEXT:{
            make_text_message(task->bot_, escaped_message, request, task->tm_->chat_id_);
        } break;
        case REPLY_TEXT:{
            make_text_message_with_reply(task->bot_, escaped_message, request, task->tm_->chat_id_, task->tm_->message_id_);
        } break;
        case REPLY_STICKER:{
            make_sticker_message_with_reply(
                task->bot_, task->bot_->monkey_stickers_[myrandom(0, task->bot_->count_monkey_stickers_ - 1)],
                request, task->tm_->chat_id_, task->tm_->message_id_
            );
        } break;
        default:
            break;
    }

    LOG(REQUEST_PATH, request, REQUEST);

    while (send_https_request(task->context_, request, response, sizeof(response)) == -1) {
        // fprintf(stderr, "Error in sending message request\n");
        while (connection_restart(task->context_, task->bot_->host_, task->bot_->port_) == -1);
    }
    
    LOG(RESPONSE_PATH, response, RESPONSE);

    free(task);
}

int telebot_init(Telebot *bot, const char *token_path) {
    bot->host_ = "api.telegram.org"; // host_
    bot->port_ = "443"; // port_
    bot->offset_ = 0; // offset_

    bot->banword_count_ = 0;
    add_banwords_to_array(bot); // banword_count_, banwords_
    
    init_rand(); // initialize random

    bot->count_monkey_answers_ = read_lines(
        PATH_TO_TEXT_ANSWERS, 
        bot->monkey_answers_, MAX_MONKEY_LINES
    ); // count_monkey_answers_, monkey_answers_
    if (bot->count_monkey_answers_ == -1) {
        perror("read from file with monkey_answers");
        return -1;
    }

    bot->count_monkey_stickers_ = read_lines(
        PATH_TO_STICKERS_ANSWERS,
        bot->monkey_stickers_, MAX_MONKEY_LINES
    ); // count_monkey_stickers_, monkey_stickers_
    if (bot->count_monkey_stickers_ == -1) {
        perror("read from file with monkey_stickers_");
        return -1;
    }

    // token_
    if (read_line_from_file(bot->token_, sizeof(bot->token_), token_path) == -1) {
        perror("read from file with token");
        return -1;
    }
    
    // connection
    if (connection_init(&bot->context_, bot->host_, bot->port_) == -1) {
        perror("connection init");
        return -1;
    }

    // threadpool
    if (threadpool_init(&bot->pool_, send_msg, bot->host_, bot->port_) == -1) {
        perror("threadpool init");
        return -1;
    }
    
    return 0;
}



void telebot_destroy(Telebot *bot) {
    threadpool_join(&bot->pool_);
    connection_destroy(&bot->context_);
    EVP_cleanup();
    LOG_DESTROY();
}


int telebot_get_updates(Telebot *bot, char *response, size_t response_size) {
    char request[REQUEST_SIZE];

    make_update_request(bot, request);

    LOG(REQUEST_PATH, request, REQUEST);

    while (send_https_request(&bot->context_, request, response, response_size) == -1) {
        // fprintf(stderr, "Error in sending getUpdates request\n");
        while (connection_restart(&bot->context_, bot->host_, bot->port_) == -1);
    }

    LOG(RESPONSE_PATH, response, RESPONSE);

    // Find last update_id
    int last_update_id = get_last_update_id(response, bot->offset_ == 0 ? 1 : 0);
    
    if (last_update_id != -1 && last_update_id != 0) {
        if (bot->offset_ == 0) {
            bot->offset_ = last_update_id + 1;
        } else {
            bot->offset_ += last_update_id;
        }
    }

    return 0;
}


int telebot_process_updates(Telebot *bot, const char *response) {
    size_t tm_size;
    TelegramMessage *messages = parse_telegram_response(response, &tm_size);

    for (size_t tm_i = 0; tm_i < tm_size; ++tm_i) {
        TelegramMessage* tm = &messages[tm_i];
        if (tm == NULL) {
            printf("No valid messages found.\n");
            return -1;
        }

        int size_bans;
        int* finder_banwords = find_banwords(bot->banwords_, bot->banword_count_, tm->text_, &size_bans);

        if (size_bans != 0) {

            // TODO: add a quote 
            // int* ids_of_symbols = get_len_of_symbols(tm->text_);
            // int normalize_begin = 0, normalize_end = 0;
            // int first_begin = finder_banwords[0], first_end = finder_banwords[1];
            // int counter_of_symbols = 0, j = 0;
            // for (int i = 0; i < strlen(tm->text_); ++i) {
            //     if (ids_of_symbols[i] == 1 || ids_of_symbols[i] == 2) {
            //         counter_of_symbols += ids_of_symbols[i];
            //         j++;
            //         if (counter_of_symbols == first_begin) {
            //             normalize_begin = j;
            //         }
            //         if (counter_of_symbols == first_end) {
            //             normalize_end = j - 1;
            //             break;
            //         }
            //     }
            // }
            // free(ids_of_symbols);
            
            MessageType type = myrandom(1, 2);
            char* monkeyword = DEFAULT_MESSAGE;
            if (type == REPLY_TEXT) {
                monkeyword = generate_random_monkey_string(bot->monkey_answers_, bot->count_monkey_answers_);    
            }
            Task* task = malloc(sizeof(Task));
            task->bot_ = bot;
            task->message_ = monkeyword;
            task->tm_ = tm;
            // task->context_: skip
            task->type_ = type;
            threadpool_submit(&bot->pool_, task);

        }

        free(finder_banwords);
    }
    return 0;
}


void make_update_request(Telebot *bot, char* request) {
    snprintf(request, REQUEST_SIZE,
             "GET /bot%s/getUpdates?timeout=100&offset=%d&limit=%d HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: C-Telegram-Bot/1.0\r\n"
             "Connection: keep-alive\r\n\r\n",
             bot->token_, bot->offset_, LIMIT_REQUESTS, bot->host_);
}


void make_text_message(
    Telebot *bot, const char *escaped_message, 
    char *request, long long chat_id)
{
    snprintf(request, REQUEST_SIZE,
             "GET /bot%s/sendMessage?chat_id=%lld&text=%s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: C-Telegram-Bot/1.0\r\n"
             "Connection: keep-alive\r\n\r\n",
             bot->token_, chat_id, escaped_message, bot->host_);
}



void make_text_message_with_reply(
    Telebot *bot, const char *escaped_message, char *request, 
    long long chat_id, long long reply_to_message_id) 
{
    snprintf(request, REQUEST_SIZE,
             "GET /bot%s/sendMessage?chat_id=%lld&text=%s&reply_to_message_id=%lld HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: C-Telegram-Bot/1.0\r\n"
             "Connection: keep-alive\r\n\r\n",
             bot->token_, chat_id, escaped_message, reply_to_message_id, bot->host_);
}

void make_sticker_message_with_reply(
    Telebot *bot, const char *id_sticker, char* request,
    long long chat_id, long long reply_to_message_id)
{

    snprintf(request, REQUEST_SIZE,
             "GET /bot%s/sendSticker?chat_id=%lld&sticker=%s&reply_to_message_id=%lld HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: C-Telegram-Bot/1.0\r\n"
             "Connection: keep-alive\r\n\r\n",
             bot->token_, chat_id, id_sticker, reply_to_message_id, bot->host_);
}


int send_https_request(connection* context, const char *request, char *response, size_t response_size) {
    int bytes;
    size_t total_bytes_read = 0;
    size_t brackets_count = 0;

    bytes = SSL_write(context->ssl_, request, strlen(request));
    if (bytes <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    while (1) {
        bytes = SSL_read(context->ssl_, response + total_bytes_read, response_size - total_bytes_read - 1);
        if (bytes <= 0) {
            int ssl_err = SSL_get_error(context->ssl_, bytes);
            if (ssl_err == SSL_ERROR_WANT_READ || ssl_err == SSL_ERROR_WANT_WRITE) {
                continue;
            } else {
                ERR_print_errors_fp(stderr);
                return -1;
            }
        }
        total_bytes_read += bytes;
        if (bytes % SSLREAD_SIZE != 0) {
            break;
        } else { // Bracket Sequences
            for (size_t i = total_bytes_read - bytes; i < total_bytes_read; ++i) {
                if (response[i] == '{') {
                    brackets_count++;
                } else if (response[i] == '}') {
                    brackets_count--;
                }
            }
            if (brackets_count == 0) {
                break;
            }
        }
    }
    response[total_bytes_read] = '\0';
    return total_bytes_read;
}


void add_banwords_to_array(Telebot *bot) {
    char ban_monkey_words[MAX_MONKEY_LINES][MAX_LINE_LEN];
    int count_of_BMW = read_lines(PATH_TO_BANWORDS, ban_monkey_words, MAX_MONKEY_LINES);
    if (count_of_BMW < 0) {
        fprintf(stderr, "Error reading banwords from file.\n");
        return;
    }

    for (int i = 0; i < count_of_BMW; i++) {
        add_banword(bot->banwords_, &bot->banword_count_, ban_monkey_words[i]);
    }
}


void normalize_url_request(const char *message, char *escaped_message, size_t escaped_size) {
    int pos = 0;
    for (int i = 0; message[i] != '\0' && pos < escaped_size - 1; i++) {
        if (message[i] == ' ') {
            escaped_message[pos++] = '+';
        } else if (message[i] == '\n') {
            escaped_message[pos++] = '%';
            escaped_message[pos++] = '0';
            escaped_message[pos++] = 'A';
        } else { 
            escaped_message[pos++] = message[i];
        }
    }
    escaped_message[pos] = '\0';
}