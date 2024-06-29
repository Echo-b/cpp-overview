#include <boost/asio.hpp>
#include "server.h"
#include "IoThreadPool.h"

int main(){
    try
    {
        auto pool = IoThreadPool::getInstance();
        boost::asio::io_context ioc;
        // elegent exit serve
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, pool](auto, auto) {
            ioc.stop();
            pool->stop();
        });
        Server s(pool->getIoService(), 9999);
        ioc.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
 
    return 0;
}