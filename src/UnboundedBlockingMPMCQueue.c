#include "UnboundedBlockingMPMCQueue.h"

// Constructor
void MPMCQueue_init(UnboundedBlockngMPMCQueue *MPMCQueue) {
    list_init(&MPMCQueue->buffer);
    pthread_mutex_init(&MPMCQueue->mutex_, NULL);
    pthread_cond_init(&MPMCQueue->not_empty_, NULL);
}

// Destructor
void MPMCQueue_clear(UnboundedBlockngMPMCQueue *MPMCQueue) {
    list_clear(&MPMCQueue->buffer);
    pthread_mutex_destroy(&MPMCQueue->mutex_);
    pthread_cond_destroy(&MPMCQueue->not_empty_);
}

void MPMCQueue_put(UnboundedBlockngMPMCQueue *MPMCQueue, void* value) {
    pthread_mutex_lock(&MPMCQueue->mutex_);
    list_push_back(&MPMCQueue->buffer, value);
    pthread_cond_signal(&MPMCQueue->not_empty_);
    pthread_mutex_unlock(&MPMCQueue->mutex_);
}

void* MPMCQueue_take(UnboundedBlockngMPMCQueue *MPMCQueue) {
    pthread_mutex_lock(&MPMCQueue->mutex_);
    while(list_size(&MPMCQueue->buffer) == 0) {
        pthread_cond_wait(&MPMCQueue->not_empty_, &MPMCQueue->mutex_);
    }
    void* retval = list_pop_front(&MPMCQueue->buffer);
    pthread_mutex_unlock(&MPMCQueue->mutex_);
    return retval;
}