#include "threadpool.h"
#include "task.h"

int threadpool_init(ThreadPool* tp , TaskFunction func, const char* host, const char* port) {
    MPMCQueue_init(&tp->tasks_);
    tp->task_function = func;
    for (int i = 0; i < THREAD_NUM; ++i) {
        if (connection_init(&tp->contexts[i], host, port, 0) == -1) {
            return -1;
        }
        ThreadPoolWithConnection *TPWC = malloc(sizeof(ThreadPoolWithConnection));
        TPWC->tp = tp;
        TPWC->context = &tp->contexts[i];
        if (pthread_create(&tp->workers_[i], NULL, WorkerRoutine, (void*)TPWC) != 0) {
            perror("Failed to create the thread");
            return -1;
        }
    }
    return 0;
}


void threadpool_submit(ThreadPool* tp, void *task) {
    MPMCQueue_put(&tp->tasks_, task);
}

int threadpool_join(ThreadPool* tp) {
    for (int i = 0; i < THREAD_NUM; ++i) {
        MPMCQueue_put(&tp->tasks_, NULL);
    }
    for (int i = 0; i < THREAD_NUM; ++i) {
        if (pthread_join(tp->workers_[i], NULL) != 0) {
            perror("Failed to join the thread");
            return -1;
        }
        connection_destroy(&tp->contexts[i]);
    }
    MPMCQueue_clear(&tp->tasks_);
    return 0;
}

void* WorkerRoutine(void* arg) {
    ThreadPoolWithConnection* TPWC = (ThreadPoolWithConnection*)arg;
    while (1) {
        Task* task = (Task*)MPMCQueue_take(&TPWC->tp->tasks_);
        if (!task) {
            break;
        }
        task->context = TPWC->context;
        TPWC->tp->task_function(task);
    }
    free(TPWC);
    return NULL;
}