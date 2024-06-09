#ifndef _THREADPOOL_
#define _THREADPOOL_
#include <pthread.h>
#include <iostream>
#include "taskq.h"
#include "taskq.cpp"
#include <cstring>
#include <unistd.h>

template<typename T>
class threadpool
{
private:
    static void* worker(void* arg);
    static void* manager(void* arg);
    void threadExit();
    
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    pthread_t* m_threadIDs;
    pthread_t m_managerID;
    TaskQueue<T>* m_taskQ;
    int m_minNum;
    int m_maxNum;
    int m_busyNum;
    int m_aliveNum;
    int m_exitNum;
    bool m_end = false;


public:
    threadpool(int maxNum, int minNum);
    ~threadpool();
    void addTask(Task<T> t);
    int getBusyNumber();
    int getAliveNumber();
    

};
#endif