#include "taskq.h"
template<typename T>
TaskQueue<T>::TaskQueue(){
    pthread_mutex_init(&m_mutex, NULL);
}

template<typename T>
TaskQueue<T>::~TaskQueue(){
    pthread_mutex_destroy(&m_mutex);
}

template<typename T>
void TaskQueue<T>::addTask(Task<T> &t){
    pthread_mutex_lock(&m_mutex);
    m_queue.push(t);
    pthread_mutex_unlock(&m_mutex);
}

template<typename T>
void TaskQueue<T>::addTask(callback func, void* arg){
    pthread_mutex_lock(&m_mutex);
    Task<T> t;
    t.arg = arg;
    t.function = func;
    m_queue.push(t);
    pthread_mutex_unlock(&m_mutex);
}

template<typename T>
int TaskQueue<T>::getTaskNumber(){
    return m_queue.size();
}

template<typename T>
Task<T> TaskQueue<T>::takeTask(){
    Task<T> t;
    pthread_mutex_lock(&m_mutex);
    if(m_queue.size() > 0){
        t = m_queue.front();
        m_queue.pop();
    }
    pthread_mutex_unlock(&m_mutex);
    return t;
}
