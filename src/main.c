#include "monkeybot.h"
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

Telebot bot;
volatile sig_atomic_t  isInitialized = 0;
pthread_t init_thread;

void *init_bot(void *arg) {
    if (telebot_init(&bot, "resources/token.txt") == -1) {
        fprintf(stderr, "Failed to initialize bot.\n");
        return NULL;
    }
    isInitialized = 1;
    printf("Bot initialized in thread.\n");
    return NULL;
}

void handle_sigint(int sig) {
    printf("\nCompletion of the program...\n");
    while (isInitialized == 0) {
        // Backoff
        sleep(1);
    }
    telebot_destroy(&bot);
    printf("Bye!\n\n");
    exit(0);
}

int main(){
    signal(SIGINT, handle_sigint);

    if (pthread_create(&init_thread, NULL, init_bot, NULL) != 0) {
        fprintf(stderr, "Failed to create init thread.\n");
        return -1;
    }
    pthread_join(init_thread, NULL);
    if (!isInitialized) {
        return -1;
    }

    printf("Bot started\n\n");
    while (1) {
        char response[RESPONSE_SIZE];
        telebot_get_updates(&bot, response, sizeof(response));
        telebot_process_updates(&bot, response);
    }
    
    telebot_destroy(&bot);
    printf("Bye!\n\n");
}