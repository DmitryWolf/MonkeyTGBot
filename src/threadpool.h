#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "unbounded_blockng_mpmc_queue.h"
#include "connection.h"

#define THREAD_NUM 16

typedef void (*task_function)(void* arg);

typedef struct {
    pthread_t workers_[THREAD_NUM];
    unbounded_blockng_mpmc_queue tasks_;
    task_function task_function_;
    connection contexts_[THREAD_NUM];
} threadpool;

int threadpool_init(threadpool* tp , task_function func, const char* host, const char* port);

void threadpool_submit(threadpool* tp, void *task);

int threadpool_join(threadpool* tp);

void* worker_routine(void* arg);

typedef struct {
    threadpool* tp;
    connection* context_;
} threadpool_with_connection;

#endif // THREADPOOL_H