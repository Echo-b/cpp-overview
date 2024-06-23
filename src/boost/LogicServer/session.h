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
#include <thread>
#include <iomanip>
#include "msgNode.h"
#include "const.h"




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
    std::queue<std::shared_ptr<sendNode>> sendQue;
    std::shared_ptr<recvNode> recvMsgNode;
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
    void send(char *msg, short maxlen, short msgid);
    void send(std::string msg, short msgid);
    void close();
    std::shared_ptr<Session> sharedSelf();
    void printRecvData(char* data, int length);
    ~Session();

private:
    void handleHeadRead(const boost::system::error_code &ec, std::size_t bytesTransfreed, std::shared_ptr<Session> shared_self);
    void handleMsgRead(const boost::system::error_code &ec, std::size_t bytesTransfreed, std::shared_ptr<Session> shared_self);
    void handleWrite(const boost::system::error_code &ec, std::shared_ptr<Session> shared_self);

};

class logicNode {
    friend class logicSystem;
public:
    logicNode(std::shared_ptr<Session> _session, std::shared_ptr<recvNode> _recvnode);
private:
    std::shared_ptr<Session> session;
    std::shared_ptr<recvNode> recvnode;
};


#endif