#ifndef __SERVER__
#define __SERVER__

#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <map>
#include "session.h"
// #include "session.cpp"

using namespace boost;

class Server
{
private:
    asio::io_context &ioc;
    asio::ip::tcp::acceptor acceptor;
    unsigned short port;
    std::map<std::string, std::shared_ptr<Session>> _sessions;
public:
    Server(asio::io_context &ioc, unsigned short port);
    void clearSession(std::string uuid);

private:
    void startAccept();
    void handleAccept(std::shared_ptr<Session> nsession, const boost::system::error_code &ec);
};



#endif