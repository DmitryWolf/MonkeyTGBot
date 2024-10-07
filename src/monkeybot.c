#include "monkeybot.h"
#include "task.h"
#include "hash.h"
#include "logger.h"

#include <unistd.h>
#include <netdb.h>
#include <openssl/err.h>

// Debug
// void print_task_info(Task* task) {
//     if (task == NULL) {
//         printf("Task is NULL\n");
//         return;
//     }
    
//     printf("Telebot pointer: %p\n", (void*)task->bot);
    
//     if (task->message != NULL) {
//         printf("Message: %s\n", task->message);
//     } else {
//         printf("Message is NULL\n");
//     }

//     printf("TelegramMessage pointer: %p\n", (void*)task->tm);

//     if (task->context != NULL) {
//         printf("Connection:\n");
//         printf("  SSL_CTX pointer: %p\n", (void*)task->context->ctx);
//         printf("  SSL pointer: %p\n", (void*)task->context->ssl);
//         printf("  Socket file descriptor: %d\n", task->context->sockfd);
//     } else {
//         printf("Connection context is NULL\n");
//     }

//     printf("Message type: %d\n", task->type);
// }

void send_msg(void* args) {
    Task* task = (Task*)(args);
    // print_task_info(task);

    char request[REQUEST_SIZE];
    char response[RESPONSE_SIZE];
    
    char escaped_message[DEFAULT_SIZE]; 
    normalize_url_request(task->message, escaped_message, sizeof(escaped_message));

    switch (task->type){
        case TEXT:
            make_text_message(task->bot, escaped_message, request, task->tm->chat_id);
            break;
        case REPLY_TEXT:
            make_text_message_with_reply(task->bot, escaped_message, request, task->tm->chat_id, task->tm->message_id);
            break;
        case REPLY_STICKER:
            make_sticker_message_with_reply(
                task->bot, task->bot->monkey_stickers[myrandom(0, task->bot->count_monkey_stickers - 1)], 
                request, task->tm->chat_id, task->tm->message_id
            );
            break;
        default:
            break;
    }
    
    LOG(REQUEST_PATH, request, 0);

    if (send_https_request(task->context, request, response, sizeof(response)) == -1) {
        // restart threadpool_worker's connection 
        connection_init(task->context, task->bot->host, task->bot->port, 1);
        int cnt_time = 1;
        while (send_https_request(task->context, request, response, sizeof(response)) == -1) {
            sleep(cnt_time);
            cnt_time = min(cnt_time + 1, TIME_TO_SLEEP);
            fprintf(stderr, "Error in sending message request\n");
            connection_init(task->context, task->bot->host, task->bot->port, 1);
        }
    }
    
    LOG(RESPONSE_PATH, response, 1);

    free(task);
}

int telebot_init(Telebot *bot, const char *token_path) {
    bot->host = "api.telegram.org";
    bot->port = "443";
    bot->offset = 0;

    bot->banword_count = 0;
    add_banwords_to_array(bot);

    init_rand();
    bot->count_monkey_answers = read_lines("resources/monkeyanswers.txt", bot->monkey_answers, MAX_MONKEY_LINES);
    bot->count_monkey_stickers = read_lines("resources/monkeystickers.txt", bot->monkey_stickers, MAX_MONKEY_LINES);

    if (read_line_from_file(bot->token, sizeof(bot->token), token_path) == -1) {
        perror("read from file with token");
        return -1;
    }

    LOG_INIT();

    threadpool_init(&bot->pool, send_msg, bot->host, bot->port);

    SSL_library_init();
    
    return connection_init(&bot->context, bot->host, bot->port, 0);
}



void telebot_destroy(Telebot *bot) {
    threadpool_join(&bot->pool);
    connection_destroy(&bot->context);
    EVP_cleanup();
    LOG_DESTROY();
}


int telebot_get_updates(Telebot *bot, char *response, size_t response_size) {
    char request[REQUEST_SIZE];
    
    make_update_request(bot, request);

    LOG(REQUEST_PATH, request, 0);;

    if (send_https_request(&bot->context, request, response, response_size) == -1) {
        connection_init(&bot->context, bot->host, bot->port, 1);
        int cnt_time = 1;
        while (send_https_request(&bot->context, request, response, response_size) == -1) {
            sleep(cnt_time);
            cnt_time = min(cnt_time + 1, TIME_TO_SLEEP);
            fprintf(stderr, "Error in sending getUpdates request\n");
            connection_init(&bot->context, bot->host, bot->port, 1);
        }
    }

    LOG(RESPONSE_PATH, response, 1);

    // Find last update_id
    char *update_id_str = response;
    int last_update_id = -1;
    while ((update_id_str = strstr(update_id_str, "\"update_id\":")) != NULL) {
        int current_update_id = atoi(update_id_str + strlen("\"update_id\":"));
        last_update_id = current_update_id;
        update_id_str += strlen("\"update_id\":");
    }

    if (last_update_id != -1) {
        bot->offset = last_update_id + 1;
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

        int sizeBans;
        int* finderBanwords = find_banwords(bot->banwords, bot->banword_count, tm->text, &sizeBans);

        if (sizeBans != 0) {

            // TODO: add a quote 
            int* ids_of_symbols = get_len_of_symbols(tm->text);
            int normalizeBegin = 0, normalizeEnd = 0;
            int firstBegin = finderBanwords[0], firstEnd = finderBanwords[1];
            int counterOfSymbols = 0, j = 0;
            for (int i = 0; i < strlen(tm->text); ++i) {
                if (ids_of_symbols[i] == 1 || ids_of_symbols[i] == 2) {
                    counterOfSymbols += ids_of_symbols[i];
                    j++;
                    if (counterOfSymbols == firstBegin) {
                        normalizeBegin = j;
                    }
                    if (counterOfSymbols == firstEnd) {
                        normalizeEnd = j - 1;
                        break;
                    }
                }
            }
            
            MessageType type = myrandom(1, 2);
            char* monkeyword = DEFAULT_MESSAGE;
            if (type == REPLY_TEXT) {
                monkeyword = generate_random_monkey_string(bot->monkey_answers, bot->count_monkey_answers);    
            }
            Task* task = malloc(sizeof(Task));
            task->bot = bot;
            task->message = monkeyword;
            task->tm = tm;
            // task->context: skip
            task->type = type;
            threadpool_submit(&bot->pool, task);

            free(ids_of_symbols);
        }

        free(finderBanwords);
    }
    return 0;
}


void make_update_request(Telebot *bot, char* request) {
    snprintf(request, REQUEST_SIZE,
             "GET /bot%s/getUpdates?timeout=100&offset=%d&limit=%d HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: C-Telegram-Bot/1.0\r\n"
             "Connection: keep-alive\r\n\r\n",
             bot->token, bot->offset, LIMIT_REQUESTS, bot->host);
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
             bot->token, chat_id, escaped_message, bot->host);
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
             bot->token, chat_id, escaped_message, reply_to_message_id, bot->host);
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
             bot->token, chat_id, id_sticker, reply_to_message_id, bot->host);
}


int send_https_request(connection* context, const char *request, char *response, size_t response_size) {
    int bytes;
    size_t total_bytes_read = 0;
    size_t brackets_count = 0;

    // if (SSL_get_error(context->ssl, bytes) == SSL_ERROR_SYSCALL) {
    //     perror("SSL_write error");
    //     ERR_print_errors_fp(stderr);
    // }
    bytes = SSL_write(context->ssl, request, strlen(request));
    if (bytes <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    while (1) {
        bytes = SSL_read(context->ssl, response + total_bytes_read, response_size - total_bytes_read - 1);
        if (bytes <= 0) {
            int ssl_err = SSL_get_error(context->ssl, bytes);
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
    return bytes;
}


void add_banwords_to_array(Telebot *bot) {
    char banmonkeywords[MAX_MONKEY_LINES][MAX_LINE_LEN];
    int count_of_BMW = read_lines("resources/banwords.txt", banmonkeywords, MAX_MONKEY_LINES);
    if (count_of_BMW < 0) {
        fprintf(stderr, "Error reading banwords from file.\n");
        return;
    }

    for (int i = 0; i < count_of_BMW; i++) {
        add_banword(bot->banwords, &bot->banword_count, banmonkeywords[i]);
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