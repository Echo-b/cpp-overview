#include <iostream>
#include <boost/asio.hpp>
#include <cstring>

using namespace boost;
const int MAXLENGTH = 1024;

int main(){
    try
    {
        boost::asio::io_context ioc;
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9999);
        asio::ip::tcp::socket sock(ioc);
        boost::system::error_code ec;
        sock.connect(ep, ec);
        if(ec == asio::error::host_not_found){
            std::cout << "connect error" << "code is:" << ec.value() << std::endl;
            return 0;
        }   

        std::cout << "Enter message:";
        char request[MAXLENGTH];
        std::cin.getline(request, MAXLENGTH);
        std::size_t reqLen = strlen(request);
        boost::asio::write(sock, asio::buffer(request, reqLen));

        char replay[MAXLENGTH];
        std::size_t replayLen = boost::asio::read(sock, asio::buffer(replay, reqLen));
        std::cout << "replay is :" << replay << std::endl; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
    
}