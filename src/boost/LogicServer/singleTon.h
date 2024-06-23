#ifndef __SIGNALTONH__
#define __SIGNALTONH__

#include <memory>
#include <mutex>
#include <iostream>

template<typename T>
class singleTon
{
protected:
    static std::shared_ptr<T> instance;
protected:
    singleTon() = default;
    singleTon(const singleTon<T> &) = delete;
    singleTon& operator=(const singleTon<T> &st) = delete;
public:
    static std::shared_ptr<T> getInstance(){
        static std::once_flag flag;
        std::call_once(flag, [&]() {
            instance = std::shared_ptr<T>(new T);
        });

        return instance;
    }

    void printAddress(){
        std::cout << "addr is" << instance.get() << std::endl;
    }
   
    ~singleTon(){
        std::cout << "singleton destruct" << std::endl;
    }
};


template<typename T>
std::shared_ptr<T> singleTon<T>::instance = nullptr;


#endif
