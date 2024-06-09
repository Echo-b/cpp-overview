#ifndef _TASKQUEUE_
#define _TASKQUEUE_

#include <iostream>
#include <queue>
#include <pthread.h>
#include "task.h"

template<typename T>
class TaskQueue {
    private:
        std::queue<Task<T>> m_queue;
        pthread_mutex_t m_mutex;

    public:
        TaskQueue();
        ~TaskQueue();
        void addTask(Task<T> &t);
        void addTask(callback func, void* arg);
        Task<T> takeTask();
        int getTaskNumber();

};

#endif