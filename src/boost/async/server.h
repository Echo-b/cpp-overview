#ifndef __SERVER__
#define __SERVER__

#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include "session.h"
#include "session.cpp"

using namespace boost;

class Server
{
private:
    asio::io_context &ioc;
    asio::ip::tcp::acceptor acceptor;
public:
    Server(asio::io_context &ioc, unsigned short port);

private:
    void startAccept();
    void handleAccept(Session* nsession, const boost::system::error_code &ec);
};




#endif