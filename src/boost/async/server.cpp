#include "server.h"

Server::Server(asio::io_context &_ioc, unsigned short _port) : ioc(_ioc), 
acceptor(_ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), _port))
{
    std::cout << "Server start success listen on port:" << _port << std::endl;
    startAccept();
}

void Server::startAccept(){
    Session* nsession = new Session(ioc);
    acceptor.async_accept(nsession->getSocket(),
    std::bind(&Server::handleAccept, this, nsession, std::placeholders::_1));
    
}
void Server::handleAccept(Session* nsession, const boost::system::error_code &ec){
    if(!ec){
        nsession->start();
    }else{
        delete nsession;
    }
    startAccept();
}
