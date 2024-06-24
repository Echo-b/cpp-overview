#include "IoServicePool.h"

IoServicePool::IoServicePool(std::size_t size):IoServices(size), works(size), nextIoService(0)
{
    for(auto i = 0; i < size; ++i){
        works[i] = std::unique_ptr<asio::io_context::work>(new asio::io_context::work(IoServices[i]));
    }

    for(auto i = 0; i < IoServices.size(); ++i){
        threads.emplace_back([this, i](){
            IoServices[i].run();
        });
    }

}

asio::io_context& IoServicePool::getIoService(){
    auto &service = IoServices[nextIoService++];
    if(nextIoService == IoServices.size()){
        nextIoService = 0;
    }
    return service;
}

void IoServicePool::stop(){
    for(auto &work : works){
        work->get_io_context().stop();
        work.reset();
    }

    for(auto &t : threads){
        t.join();
    }
}

IoServicePool::~IoServicePool()
{
    std::cout << "IOServicePool destruct" << std::endl;
}