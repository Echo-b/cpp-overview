#include <boost/asio.hpp>
#include "server.h"
#include "server.cpp"
#include "IoServicePool.h"
#include "IoServicePool.cpp"


int main(){
    try
    {
        auto pool = IoServicePool::getInstance();
        boost::asio::io_context ioc;
        // elegent exit serve
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, pool](auto, auto) {
            ioc.stop();
            pool->stop();
        });
        Server s(ioc, 9999);
        ioc.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
 
    return 0;
}