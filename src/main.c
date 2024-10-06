#include "monkeybot.h"
#include <signal.h>
#include <unistd.h>

Telebot bot;

// void handle_sigint(int sig) {
//     printf("Caught signal %d (Ctrl+C). Exiting gracefully...\n", sig);
//     telebot_destroy(&bot);
//     exit(0);
// }

void SignalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        telebot_destroy(&bot);
        printf("Bye!\n\n");
    }
}

int main(){
    // signal(SIGINT, SignalHandler);
    // signal(SIGTERM, SignalHandler);

    if (telebot_init(&bot, "resources/token.txt") == -1) {
        return -1;
    }
    
    printf("Bot started\n\n");
    while (1) {
        char response[2*RESPONSE_SIZE];
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