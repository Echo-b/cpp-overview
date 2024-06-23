#include <boost/asio.hpp>
#include "server.h"
#include "server.cpp"

int main(){
    try
    {
        boost::asio::io_context ioc;
        // elegent exit serve
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](auto, auto) {
            ioc.stop();
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