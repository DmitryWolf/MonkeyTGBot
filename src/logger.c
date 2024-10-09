#include "logger.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t mutex_log_ = PTHREAD_MUTEX_INITIALIZER;

void LOG_INIT() {
    pthread_mutex_init(&mutex_log_, NULL);
}

char* LOG_DUMP() {
    const size_t buffer_size = 1024;
    char* dump_text = (char*)malloc(buffer_size);
    if (dump_text == NULL) {
        perror("Error in malloc in LOG_DUMP");
        exit(EXIT_FAILURE);
    }
    memset(dump_text, 0, buffer_size);

    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);

    snprintf(dump_text, buffer_size, "Thread Info: Main thread ID: %lu\n", pthread_self());
    snprintf(dump_text + strlen(dump_text), buffer_size - strlen(dump_text), 
             "Scheduling Policy: %d\n", policy);
    snprintf(dump_text + strlen(dump_text), buffer_size - strlen(dump_text), 
             "Thread Priority: %d\n", param.sched_priority);

    sigset_t sigset;
    sigemptyset(&sigset);

    if (pthread_sigmask(SIG_SETMASK, NULL, &sigset) == 0) {
        snprintf(dump_text + strlen(dump_text), buffer_size - strlen(dump_text), "Blocked Signals: ");
        for (int i = 1; i < NSIG; ++i) {
            if (sigismember(&sigset, i)) {
                snprintf(dump_text + strlen(dump_text), buffer_size - strlen(dump_text), "%d ", i);
            }
        }
        snprintf(dump_text + strlen(dump_text), buffer_size - strlen(dump_text), "\n");
    } else {
        snprintf(dump_text + strlen(dump_text), buffer_size - strlen(dump_text), "Failed to retrieve signal mask.\n");
    }
    
    return dump_text;
}

void LOG(const char *path, const char *text, LOG_TYPE type) {
    pthread_mutex_lock(&mutex_log_);

    const char *body;
    switch(type) {
        case REQUEST:{
            body = text;
        } break;
        case RESPONSE:{
            body = strstr(text, "\r\n\r\n");
            if (!body) {
                printf("Error: The response body was not found\n");
                body = text;
            } else {
                // Skip "\r\n\r\n"
                body += 4;
            }
        } break;
        case ERROR:{
            body = LOG_DUMP();
        } break;
        case SMTH:{
            body = LOG_DUMP();
        } break;
        default: 
            break;
    }
    size_t size = strlen(body);

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char time_buffer[20];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);


    FILE *file = fopen(path, "a");
    if (!file) {
        fprintf(stderr, "Error in opening log-file\n");
        return;
    }
    fprintf(file, "DUMP [%s] : ", time_buffer);
    fwrite(body, 1, size, file);
    fprintf(file, "\n");
    fclose(file);


    pthread_mutex_unlock(&mutex_log_);
    
}

void LOG_DESTROY() {
    pthread_mutex_destroy(&mutex_log_);
}