#include "IoThreadPool.h"

IoThreadPool::IoThreadPool(std::size_t size):work(new boost::asio::io_context::work(service))
{
    for(int i = 0; i < size; ++i){
        threads.emplace_back([this](){
            service.run();
        });
    }
}

void IoThreadPool::stop(){
    service.stop();
    work.reset();

    for(auto &t : threads){
        t.join();
    }
}

boost::asio::io_context& IoThreadPool::getIoService(){
    return service;
}


IoThreadPool::~IoThreadPool()
{
}