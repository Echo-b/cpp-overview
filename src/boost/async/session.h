#ifndef __SESSION__
#define __SESSION__

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <cstring>

using namespace boost;

class Session
{
private:
    asio::ip::tcp::socket sock;
    enum {maxLen = 1024};
    char data[maxLen];
public:
    Session(asio::io_context &ioc) : sock(ioc){};
    asio::ip::tcp::socket &getSocket(){
        return sock;
    }
    void start();

private:
    void handleRead(const boost::system::error_code &ec, std::size_t bytesTransfreed);
    void handleWrite(const boost::system::error_code &ec);
};


#endif