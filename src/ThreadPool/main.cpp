#include <iostream>
#include "threadpool.h"
#include "threadpool.cpp"

void taskFunction(void *arg){
    int num = *(int *)arg;
    std::cout << "thread " << std::to_string(pthread_self()) << "is working.. number=" << num << std::endl;
    sleep(1);

}   


int main(){
    threadpool<int> pool(10,3);
    for(int i = 0; i < 100; ++i){
        int *num = new int(i+100);
        pool.addTask(Task<int>(taskFunction, num));

    }
    sleep(20);
    return 0;
}