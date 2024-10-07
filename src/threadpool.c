#include "threadpool.h"
#include "task.h"

int threadpool_init(threadpool* tp , task_function func, const char* host, const char* port) {
    mpmcqueue_init(&tp->tasks_);
    tp->task_function_ = func;
    for (int i = 0; i < THREAD_NUM; ++i) {
        if (connection_init(&tp->contexts_[i], host, port) == -1) {
            return -1;
        }
        threadpool_with_connection *TPWC = malloc(sizeof(threadpool_with_connection));
        TPWC->tp = tp;
        TPWC->context_ = &tp->contexts_[i];
        if (pthread_create(&tp->workers_[i], NULL, worker_routine, (void*)TPWC) != 0) {
            perror("Failed to create the thread");
            return -1;
        }
    }
    return 0;
}


void threadpool_submit(threadpool* tp, void *task) {
    mpmcqueue_put(&tp->tasks_, task);
}

int threadpool_join(threadpool* tp) {
    for (int i = 0; i < THREAD_NUM; ++i) {
        mpmcqueue_put(&tp->tasks_, NULL);
    }
    for (int i = 0; i < THREAD_NUM; ++i) {
        if (pthread_join(tp->workers_[i], NULL) != 0) {
            perror("Failed to join the thread");
            return -1;
        }
        connection_destroy(&tp->contexts_[i]);
    }
    mpmcqueue_clear(&tp->tasks_);
    return 0;
}

void* worker_routine(void* arg) {
    threadpool_with_connection* TPWC = (threadpool_with_connection*)arg;
    while (1) {
        Task* task = (Task*)mpmcqueue_take(&TPWC->tp->tasks_);
        if (!task) {
            break;
        }
        task->context_ = TPWC->context_;
        TPWC->tp->task_function_(task);
    }
    free(TPWC);
    return NULL;
}