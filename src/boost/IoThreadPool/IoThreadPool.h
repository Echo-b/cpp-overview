#ifndef __IOTHREADPOOL__
#define __IOTHREADPOOL__

#include <boost/asio.hpp>
#include <mutex>
#include <vector>
#include "singleTon.h"

class IoThreadPool:public singleTon<IoThreadPool>
{
    friend class singleTon<IoThreadPool>;
private:
    boost::asio::io_context service;
    std::unique_ptr<boost::asio::io_context::work> work;
    std::vector<std::thread> threads;

private:
    IoThreadPool(std::size_t size = std::thread::hardware_concurrency());
public:
    IoThreadPool(const IoThreadPool&) = delete;
    IoThreadPool& operator=(const IoThreadPool&) = delete;
    void stop();
    boost::asio::io_context& getIoService();
    ~IoThreadPool();
};


#endif