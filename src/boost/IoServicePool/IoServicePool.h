#ifndef __IOSERVICEPOOL__
#define __IOSERVICEPOOL__

#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include "singleTon.h"
#include <iostream>

using namespace boost;

class IoServicePool:public singleTon<IoServicePool>
{
    friend singleTon<IoServicePool>;
private:
    std::vector<asio::io_context> IoServices;
    std::vector<std::unique_ptr<asio::io_context::work>> works;
    std::vector<std::thread> threads;
    std::size_t nextIoService;

private:
    IoServicePool(std::size_t size = std::thread::hardware_concurrency());

public:
    IoServicePool(const IoServicePool&) = delete;
    IoServicePool& operator=(const IoServicePool&) = delete;
    asio::io_context &getIoService();
    void stop();
    ~IoServicePool();
};





#endif