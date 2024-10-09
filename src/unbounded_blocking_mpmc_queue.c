#include "unbounded_blocking_mpmc_queue.h"

// Constructor
void mpmcqueue_init(unbounded_blockng_mpmc_queue *mpmc_queue) {
    list_init(&mpmc_queue->buffer);
    pthread_mutex_init(&mpmc_queue->mutex, NULL);
    pthread_cond_init(&mpmc_queue->not_empty, NULL);
}

// Destructor
void mpmcqueue_clear(unbounded_blockng_mpmc_queue *mpmc_queue) {
    list_clear(&mpmc_queue->buffer);
    pthread_mutex_destroy(&mpmc_queue->mutex);
    pthread_cond_destroy(&mpmc_queue->not_empty);
}

void mpmcqueue_put(unbounded_blockng_mpmc_queue *mpmc_queue, void* value) {
    pthread_mutex_lock(&mpmc_queue->mutex);
    list_push_back(&mpmc_queue->buffer, value);
    pthread_cond_signal(&mpmc_queue->not_empty);
    pthread_mutex_unlock(&mpmc_queue->mutex);
}

void* mpmcqueue_take(unbounded_blockng_mpmc_queue *mpmc_queue) {
    pthread_mutex_lock(&mpmc_queue->mutex);
    while(list_size(&mpmc_queue->buffer) == 0) {
        pthread_cond_wait(&mpmc_queue->not_empty, &mpmc_queue->mutex);
    }
    void* retval = list_pop_front(&mpmc_queue->buffer);
    pthread_mutex_unlock(&mpmc_queue->mutex);
    return retval;
}