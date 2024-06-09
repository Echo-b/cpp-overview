#include "threadpool.h"
template<typename T>
threadpool<T>::threadpool(int maxNum, int minNum)
{
    this->m_taskQ = new TaskQueue<T>();
    do {
        this->m_maxNum = maxNum;
        this->m_minNum = minNum;
        this->m_aliveNum = minNum;
        this->m_busyNum = 0;

        this->m_threadIDs = new pthread_t[maxNum];
        if(this->m_threadIDs == nullptr){
            std::cout << "malloc thread fail" << std::endl;
            break;
        }
        memset(m_threadIDs, -1 ,sizeof(pthread_t*) * maxNum);
        if(pthread_mutex_init(&m_mutex, NULL) != 0 
        || pthread_cond_init(&m_cond, NULL) != 0){
            std::cout << "init mutex or condition fail" << std::endl;
            break;
        }
        
        for( int i = 0; i < minNum; ++i){
            pthread_create(&m_threadIDs[i],NULL, worker, this);
            std::cout << "creat son thread ID: " << std::to_string(m_threadIDs[i]) << std::endl;
        }

        pthread_create(&m_managerID, NULL, manager, this);

    } while(0);
    
}

template<typename T>
threadpool<T>::~threadpool()
{
    m_end = true;
    pthread_join(m_managerID, NULL);

    for( int i = 0; i < m_aliveNum; ++i){
        pthread_cond_signal(&m_cond);
    }
    if(m_taskQ) delete m_taskQ;
    if(m_threadIDs) delete[] m_threadIDs;
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

template<typename T>
void threadpool<T>::addTask(Task<T> t){

    if(m_end) return;
    m_taskQ->addTask(t); // 不满， 唤醒阻塞线程工作
    pthread_cond_signal(&m_cond);

}

template<typename T>
int threadpool<T>::getAliveNumber(){
    int ret;
    pthread_mutex_lock(&m_mutex);
    ret = m_aliveNum;
    pthread_mutex_unlock(&m_mutex);
    return ret;
}

template<typename T>
int threadpool<T>::getBusyNumber(){
    int ret;
    pthread_mutex_lock(&m_mutex);
    ret = m_busyNum;
    pthread_mutex_unlock(&m_mutex);
    return ret;
}

template<typename T>
void* threadpool<T>::manager(void* arg){
    threadpool* pool = static_cast<threadpool*>(arg);
    while(!pool->m_end){
        sleep(5);
        pthread_mutex_lock(&pool->m_mutex);
        int qSize = pool->m_taskQ->getTaskNumber();
        int liveNum = pool->m_aliveNum;
        int busyNum = pool->m_busyNum;
        pthread_mutex_unlock(&pool->m_mutex);

        const int NUMBER = 2;
        if (qSize > liveNum && liveNum < pool->m_maxNum){
            pthread_mutex_lock(&pool->m_mutex);
            int num = 0;
            for(int i = 0; i < pool->m_maxNum && num < NUMBER 
            && pool->m_aliveNum < pool->m_maxNum; ++i){
                if(pool->m_threadIDs[i] == -1){
                    pthread_create(&pool->m_threadIDs[i], NULL, worker, pool);
                    num++;
                    pool->m_aliveNum++;
                }

            }
            pthread_mutex_unlock(&pool->m_mutex);
        }

        if(busyNum * 2 < liveNum && liveNum > pool->m_minNum){
            pthread_mutex_lock(&pool->m_mutex);
            pool->m_exitNum = NUMBER;
            pthread_mutex_unlock(&pool->m_mutex);

            for(int i = 0; i < NUMBER; ++i){
                pthread_cond_signal(&pool->m_cond);
            }
        }
    }
    return nullptr;
}

template<typename T>
void* threadpool<T>::worker(void* arg){
    threadpool* pool = static_cast<threadpool*>(arg);
    while (true)
    {   
        pthread_mutex_lock(&pool->m_mutex);
        while(pool->m_taskQ->getTaskNumber() == 0 && !pool->m_end){
            std::cout << "thread " << std::to_string(pthread_self()) << "waiting..." << std::endl;
            pthread_cond_wait(&pool->m_cond, &pool->m_mutex);

            if(pool->m_exitNum > 0){
                pool->m_exitNum--;
                if(pool->m_aliveNum > pool->m_minNum){
                    pool->m_aliveNum--;
                    pthread_mutex_unlock(&pool->m_mutex);
                    pool->threadExit();
                }
            }
        }

        if(pool->m_end){
            pthread_mutex_unlock(&pool->m_mutex);
            pool->threadExit();
        }

        Task<T> task = pool->m_taskQ->takeTask();
        pool->m_busyNum++;
        pthread_mutex_unlock(&pool->m_mutex);

        std::cout << "thread " << std::to_string(pthread_self()) << " start working..." << std::endl;
        task.function(task.arg);
        // delete task.arg;
        // task.arg = nullptr;
        std::cout << "thead " << std::to_string(pthread_self()) << " end working..." << std::endl;
        
        pthread_mutex_lock(&pool->m_mutex);
        pool->m_busyNum--;
        pthread_mutex_unlock(&pool->m_mutex);
    }
    return nullptr;
}

template<typename T>
void threadpool<T>::threadExit(){
    pthread_t tid = pthread_self();
    for(int i = 0; i < m_maxNum; ++i){
        if(m_threadIDs[i] == tid){
            std::cout << "threadExit function call thread exit ID: " << std::to_string(pthread_self()) << std::endl;
            m_threadIDs[i] = -1;
            break;
        }
    }
    pthread_exit(NULL);
}