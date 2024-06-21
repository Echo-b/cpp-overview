#include <boost/asio.hpp>
#include "server.h"
#include "server.cpp"

int main(){
    try
    {
        boost::asio::io_context ioc;
        Server s(ioc, 9999);
        ioc.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
 
    return 0;
}