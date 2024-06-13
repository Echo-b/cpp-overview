#include <iostream>
#include <boost/asio.hpp>
const int MAX_LENGTH  = 1024;

using namespace boost;

int main(){
    try
    {
        asio::io_context ioc;
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9999);
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        
        boost::system::error_code ec;
        sock.connect(ep, ec);
        if(ec == asio::error::host_not_found){
            std::cout << "connect failed" << std::endl;
            return 0;
        }

        std::cout << "Enter Message:";
        char request[MAX_LENGTH];
        std::cin.getline(request, MAX_LENGTH);
        std::size_t requestLen = strlen(request);

        asio::write(sock, asio::buffer(request, requestLen));

        char replay[MAX_LENGTH];
        std::size_t replayLen = asio::read(sock, asio::buffer(replay, requestLen));

        std::cout << "replay is :";
        std::cout.write(replay, replayLen) << std::endl; 

        // boost::system::system_error ec = asio::error::host_not_found;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}