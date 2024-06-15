#include "session.h"

void Session::handleRead(const boost::system::error_code &ec, std::size_t bytesTransfreed){
    if(!ec){
        // std::cout << "enter session::handleRead()" << std::endl;
        std::cout << "server receive data is:"  << data << std::endl;
        // std::cout << "byteTransfreed value is:" << bytesTransfreed << std::endl;
        boost::asio::async_write(sock, asio::buffer(data, bytesTransfreed), 
        std::bind(&Session::handleWrite, this, std::placeholders::_1));
    }else{
        delete this;
    }
}

void Session::handleWrite(const boost::system::error_code &ec){
    if(!ec){
        // std::cout << "enter session::handleWrite()" << std::endl;
        memset(data, 0, maxLen);
        this->sock.async_read_some(asio::buffer(data, maxLen),
        std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2));
    }else{
        delete this;
    }
}

void Session::start(){
    // std::cout << "enter session::start()" << std::endl;
    memset(data, 0, maxLen);
    this->sock.async_read_some(asio::buffer(data, maxLen),
    std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2));
}