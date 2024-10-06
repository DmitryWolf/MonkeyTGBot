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
        json_t *message = json_object_get(update, "message");
        if (message) {
            // Get message_id
            json_t *message_id = json_object_get(message, "message_id");
            if (json_is_integer(message_id)) {
                messages[index].message_id = json_integer_value(message_id);
            }

            // Get info of user
            json_t *from = json_object_get(message, "from");
            if (from) {
                json_t *from_id = json_object_get(from, "id");
                json_t *first_name = json_object_get(from, "first_name");
                json_t *username = json_object_get(from, "username");
                if (json_is_integer(from_id)) {
                    messages[index].from_id = json_integer_value(from_id);
                }
                if (json_is_string(first_name)) {
                    strncpy(messages[index].first_name, json_string_value(first_name), sizeof(messages[index].first_name) - 1);
                    messages[index].first_name[sizeof(messages[index].first_name) - 1] = '\0'; // Overflow protection
                }
                if (json_is_string(username)) {
                    strncpy(messages[index].username, json_string_value(username), sizeof(messages[index].username) - 1);
                    messages[index].username[sizeof(messages[index].username) - 1] = '\0'; // Overflow protection
                }
            }

            // Get info of chat
            json_t *chat = json_object_get(message, "chat");
            if (chat) {
                json_t *chat_id = json_object_get(chat, "id");
                json_t *chat_title = json_object_get(chat, "title");
                if (json_is_integer(chat_id)) {
                    messages[index].chat_id = json_integer_value(chat_id);
                }
                if (json_is_string(chat_title)) {
                    strncpy(messages[index].chat_title, json_string_value(chat_title), sizeof(messages[index].chat_title) - 1);
                    messages[index].chat_title[sizeof(messages[index].chat_title) - 1] = '\0'; // Overflow protection
                }
            }

            // Get text
            json_t *text = json_object_get(message, "text");
            if (json_is_string(text)) {
                strncpy(messages[index].text, json_string_value(text), sizeof(messages[index].text) - 1);
                messages[index].text[sizeof(messages[index].text) - 1] = '\0'; // Overflow protection
            }
        }
    }

    // free memory
    json_decref(root);
    return messages;
}