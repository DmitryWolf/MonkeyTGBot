#include "parser.h"

#include <string.h>

TelegramMessage* parse_telegram_response(const char *response, size_t *size) {
    // Find JSON body
    const char *json_body = strstr(response, "\r\n\r\n");
    if (!json_body) {
        printf("Error: The response body was not found\n");
        return NULL;
    }

    // Skip "\r\n\r\n"
    json_body += 4;

    // -----Parsing-----

    json_t *root;
    json_error_t error;

    root = json_loads(json_body, 0, &error);
    if (!root) {
        fprintf(stderr, "Error in parsing JSON: %s\n", error.text);
        return NULL;
    }

    json_t *ok_field = json_object_get(root, "ok");
    if (!json_is_true(ok_field)) {
        printf("Error in request to JSON API.\n");
        json_decref(root);
        return NULL;
    }

    json_t *result = json_object_get(root, "result");
    if (!result) {
        printf("Result is empty.\n");
        json_decref(root);
        return NULL;
    }

    if (!json_is_array(result)) {
        printf("Result is not an array (type: %d).\n", json_typeof(result));
        json_decref(root);
        return NULL;
    }

    size_t count = json_array_size(result);
    *size = count;
    TelegramMessage *messages = malloc(count * sizeof(TelegramMessage));
    if (!messages) {
        printf("Error in malloc (json messages).\n");
        json_decref(root);
        return NULL;
    }

    // Iterate into the array
    size_t index;
    json_t *update;
    json_array_foreach(result, index, update) {
        if (json_is_object(update)) {
            void *iter = json_object_iter(update);
            size_t idx = 0;
            while (iter) {
                const char *key = json_object_iter_key(iter);
                json_t *value = json_object_iter_value(iter);
                if (idx == 1) {
                    if (strcmp(key, "message") != 0) {
                        messages[index].message_id_ = -1;
                        break;
                    } else {
                        // Get message_id_
                        json_t *message_id = json_object_get(value, "message_id");
                        if (json_is_integer(message_id)) {
                            messages[index].message_id_ = json_integer_value(message_id);
                        } else {
                            messages[index].message_id_ = -1;
                        }
                        
                        // Get info of user
                        json_t *from = json_object_get(value, "from");
                        if (from) {
                            json_t *from_id = json_object_get(from, "id");
                            json_t *first_name = json_object_get(from, "first_name");
                            json_t *username = json_object_get(from, "username");
                            if (json_is_integer(from_id)) {
                                messages[index].from_id_ = json_integer_value(from_id);
                            } else {
                                messages[index].from_id_ = -1;
                            }
                            if (json_is_string(first_name)) {
                                strncpy(messages[index].first_name_, json_string_value(first_name), sizeof(messages[index].first_name_) - 1);
                                messages[index].first_name_[sizeof(messages[index].first_name_) - 1] = '\0'; // Overflow protection
                            } else {
                                messages[index].first_name_[0] = '\0';
                            }
                            if (json_is_string(username)) {
                                strncpy(messages[index].username_, json_string_value(username), sizeof(messages[index].username_) - 1);
                                messages[index].username_[sizeof(messages[index].username_) - 1] = '\0'; // Overflow protection
                            } else {
                                messages[index].username_[0] = '\0';
                            }
                        }

                        // Get info of chat
                        json_t *chat = json_object_get(value, "chat");
                        if (chat) {
                            json_t *chat_id = json_object_get(chat, "id");
                            json_t *chat_title = json_object_get(chat, "title");
                            if (json_is_integer(chat_id)) {
                                messages[index].chat_id_ = json_integer_value(chat_id);
                            } else {
                                messages[index].chat_id_ = -1;
                            }
                            if (json_is_string(chat_title)) {
                                strncpy(messages[index].chat_title_, json_string_value(chat_title), sizeof(messages[index].chat_title_) - 1);
                                messages[index].chat_title_[sizeof(messages[index].chat_title_) - 1] = '\0'; // Overflow protection
                            } else {
                                messages[index].chat_title_[0] = '\0';
                            }
                        }

                        // Get text
                        json_t *text = json_object_get(value, "text");
                        if (json_is_string(text)) {
                            strncpy(messages[index].text_, json_string_value(text), sizeof(messages[index].text_) - 1);
                            messages[index].text_[sizeof(messages[index].text_) - 1] = '\0'; // Overflow protection
                        } else {
                            messages[index].text_[0] = '\0';
                        }
                    }
                }
                iter = json_object_iter_next(update, iter);
                idx++;
            }
        }
    }

    // free memory
    json_decref(root);
    return messages;
}

int get_last_update_id(const char *response, int is_first_parsing) {
    // Find JSON body
    const char *json_body = strstr(response, "\r\n\r\n");
    if (!json_body) {
        printf("Error: The response body was not found\n");
        return -1;
    }

    // Skip "\r\n\r\n"
    json_body += 4;

    // -----Parsing-----

    json_t *root;
    json_error_t error;

    root = json_loads(json_body, 0, &error);
    if (!root) {
        fprintf(stderr, "Error in parsing JSON: %s\n", error.text);
        return -1;
    }

    json_t *ok_field = json_object_get(root, "ok");
    if (!json_is_true(ok_field)) {
        printf("Error in request to JSON API.\n");
        json_decref(root);
        return -1;
    }

    json_t *result = json_object_get(root, "result");
    if (!result) {
        printf("Result is empty.\n");
        json_decref(root);
        return -1;
    }

    if (!json_is_array(result)) {
        printf("Result is not an array (type: %d).\n", json_typeof(result));
        json_decref(root);
        return -1;
    }

    int count = json_array_size(result);

    size_t index;
    json_t *update;
    int last_update_id = 0;
    if (is_first_parsing) {
        json_array_foreach(result, index, update) {
            json_t *update_id = json_object_get(update, "update_id");
            if (json_is_integer(update_id)) {
                last_update_id = json_integer_value(update_id);
            }
        }
    }

    json_decref(root);
    if (is_first_parsing) {
        count = last_update_id;
    }
    return count;
}