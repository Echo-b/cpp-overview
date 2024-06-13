#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <cstdlib>
#include <set>
const int MAX_LENGTH  = 1024;

using namespace boost;
typedef std::shared_ptr<asio::ip::tcp::socket> socket_ptr;
std::set<std::shared_ptr<std::thread>> thread_set;

void session(socket_ptr sock){
    try
    {
        for (;;)
        {
            char data[MAX_LENGTH];
            memset(data, '\0', MAX_LENGTH);
            boost::system::error_code ec;
            size_t length = sock->read_some(asio::buffer(data, MAX_LENGTH));
            if(ec == asio::error::eof){
                std::cout << "connection close by peer" << std::endl;
                break;
            }
            else if(ec){
                throw boost::system::system_error(ec);
            }
            std::cout << "receive from:" << sock->remote_endpoint().address().to_string() << std::endl;
            std::cout << "receive message is:" << data << std::endl;
            asio::write(*sock, asio::buffer(data, length));
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void server(asio::io_context &ioc, unsigned short port){
    asio::ip::tcp::acceptor acp(ioc, asio::ip::tcp::endpoint(asio::ip::address_v4().any(), port));
    for(;;){
        socket_ptr sock(new asio::ip::tcp::socket(ioc));
        acp.accept(*sock);
        auto t = std::make_shared<std::thread>(session, sock);
        thread_set.insert(t);
    }
}

int main(){
    try
    {
        asio::io_context ioc;
        server(ioc, 9999);
        for(auto &t : thread_set){
            t->join();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}