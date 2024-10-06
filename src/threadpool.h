#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "UnboundedBlockingMPMCQueue.h"
#include "connection.h"

#define THREAD_NUM 16

typedef void (*TaskFunction)(void* arg);

typedef struct {
    pthread_t workers_[THREAD_NUM];
    UnboundedBlockngMPMCQueue tasks_;
    TaskFunction task_function;
    connection contexts[THREAD_NUM];
} ThreadPool;

int threadpool_init(ThreadPool* tp , TaskFunction func, const char* host, const char* port);

void threadpool_submit(ThreadPool* tp, void *task);

int threadpool_join(ThreadPool* tp);

void* WorkerRoutine(void* arg);

typedef struct {
    ThreadPool* tp;
    connection* context;
} ThreadPoolWithConnection;

#endif // THREADPOOL_H