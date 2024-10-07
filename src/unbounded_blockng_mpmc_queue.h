#ifndef UNBOUNDED_BLOCKING_MPMC_QUEUE_H
#define UNBOUNDED_BLOCKING_MPMC_QUEUE_H

#include "list.h"
#include <pthread.h>

typedef struct {
    List buffer;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty; 
} unbounded_blockng_mpmc_queue;

void mpmcqueue_init(unbounded_blockng_mpmc_queue *mpmc_queue);

void mpmcqueue_clear(unbounded_blockng_mpmc_queue *mpmc_queue);

void mpmcqueue_put(unbounded_blockng_mpmc_queue *mpmc_queue, void *value);

void* mpmcqueue_take(unbounded_blockng_mpmc_queue *mpmc_queue);

#endif // UNBOUNDED_BLOCKING_MPMC_QUEUE_H