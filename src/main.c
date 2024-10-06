#include "monkeybot.h"
#include <signal.h>
#include <unistd.h>


int main(){
    Telebot bot;

    if (telebot_init(&bot, "resources/token.txt") == -1) {
        return -1;
    }
    
    printf("Bot started\n\n");
    while (1) {
        char response[RESPONSE_SIZE];
        if (telebot_get_updates(&bot, response, sizeof(response)) == 0) {
            telebot_process_updates(&bot, response);
        } else { 
            fprintf(stderr, "Failed to get updates.\n");
            // restart connection
            if (connection_init(&bot.context, bot.host, bot.port, 1) == -1) {
                fprintf(stderr, "Failed to restart connection.\n");
                sleep(1);
            }
            bot.offset-=2; // move offset to resend message
        }
    }
    
    telebot_destroy(&bot);
    printf("Bye!\n\n");
}