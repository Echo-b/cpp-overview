#include "server.h"
#include "session.cpp"

Server::Server(asio::io_context &_ioc, unsigned short _port) : ioc(_ioc), 
acceptor(_ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), _port))
{
    std::cout << "Server start success listen on port:" << _port << std::endl;
    startAccept();
}

void Server::startAccept(){
    // Session* nsession = new Session(ioc);
    std::shared_ptr<Session> nsession = std::make_shared<Session>(ioc, this);
    acceptor.async_accept(nsession->getSocket(),
    std::bind(&Server::handleAccept, this, nsession, std::placeholders::_1));
    
}
void Server::handleAccept(std::shared_ptr<Session> nsession, const boost::system::error_code &ec){
    if(!ec){
        nsession->start();
        _sessions.insert(std::make_pair(nsession->getUuid(), nsession));
    }else{
        std::cout << "session accept failed error is:" << ec.what() << std::endl;
    }
    startAccept();
}

void Server::clearSession(std::string uuid){
    _sessions.erase(uuid);
}
