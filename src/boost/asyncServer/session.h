#ifndef __SESSION__
#define __SESSION__

#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <boost/asio.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cstring>
#include "msgNode.h"


#define MAX_LENGTH  1024*2

using namespace boost;
class Server;

class Session: public std::enable_shared_from_this<Session>
{
private:
    asio::ip::tcp::socket sock;
    std::string uuid;
    char data[MAX_LENGTH];
    Server *server;
    bool bclose;
    std::queue<std::shared_ptr<msgNode>> sendQue;
    std::shared_ptr<msgNode> recvMsgNode;
    std::shared_ptr<msgNode> recvHeadNode;
    std::mutex smutex;
    bool bHeadParse;
public:
    Session(asio::io_context &ioc, Server *_server);
    asio::ip::tcp::socket &getSocket(){
        return sock;
    }
    std::string &getUuid();
    void start();
    void send(char *msg, int maxlen);
    void close();
    std::shared_ptr<Session> sharedSelf();
    ~Session();

private:
    void handleRead(const boost::system::error_code &ec, std::size_t bytesTransfreed, std::shared_ptr<Session> shared_self);
    void handleWrite(const boost::system::error_code &ec, std::shared_ptr<Session> shared_self);
};


#endif

