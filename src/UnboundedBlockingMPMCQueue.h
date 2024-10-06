#ifndef UNBOUNDED_BLOCKING_MPMC_QUEUE_H
#define UNBOUNDED_BLOCKING_MPMC_QUEUE_H

#include "list.h"
#include <pthread.h>

typedef struct {
    List buffer;
    pthread_mutex_t mutex_;
    pthread_cond_t not_empty_; 
} UnboundedBlockngMPMCQueue;

void MPMCQueue_init(UnboundedBlockngMPMCQueue *MPMCQueue);

void MPMCQueue_clear(UnboundedBlockngMPMCQueue *MPMCQueue);

void MPMCQueue_put(UnboundedBlockngMPMCQueue *MPMCQueue, void *value);

void* MPMCQueue_take(UnboundedBlockngMPMCQueue *MPMCQueue);

#endif // UNBOUNDED_BLOCKING_MPMC_QUEUE_H