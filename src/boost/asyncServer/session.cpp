#include "session.h"
#include "server.h"
// #include "server.cpp"

Session::Session(asio::io_context &ioc, Server *_server): sock(ioc), server(_server), bclose(false), bHeadParse(false){
    boost::uuids::uuid auuid = boost::uuids::random_generator()();
    uuid = boost::uuids::to_string(auuid);
    recvMsgNode = std::make_shared<msgNode>(HEAD_LENGTH);
}

Session::~Session(){
    std::cout << "destruct session" << std::endl;
}


void Session::handleRead(const boost::system::error_code &ec, std::size_t bytesTransfreed, std::shared_ptr<Session> shared_self){
    if(!ec){
        // std::cout << "enter session::handleRead()" << std::endl;
        std::cout << "server receive data is:"  << data << std::endl;
        // std::cout << "byteTransfreed value is:" << bytesTransfreed << std::endl;
        send(data, bytesTransfreed);
        memset(data, 0, bytesTransfreed);
        sock.async_read_some(asio::buffer(data, MAX_LENGTH), 
        std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
    }else{
        std::cout << "handle read failed error is:" << ec.what() << std::endl;
        server->clearSession(uuid);
    }
}

void Session::handleWrite(const boost::system::error_code &ec, std::shared_ptr<Session> shared_self){
    if(!ec){
        std::unique_lock<std::mutex> readMutex(smutex);
        sendQue.pop();
        if(!sendQue.empty()){
            auto &msgnode = sendQue.front();
            boost::asio::async_write(sock, asio::buffer(msgnode->data, msgnode->totalLen),
            std::bind(&Session::handleWrite, this, std::placeholders::_1, shared_self));
        }
    }else{
        std::cout << "handle write failed error is:" << ec.what() << std::endl;
        server->clearSession(uuid);
    }
}

void Session::start(){
    memset(data, 0, MAX_LENGTH);
    this->sock.async_read_some(asio::buffer(data, MAX_LENGTH),
    std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf()));
}

std::string &Session::getUuid(){
    return uuid;
}

void Session::send(char *msg, int maxlen){
    bool pending = false;
    std::unique_lock<std::mutex> sendMutex(smutex);
    if(sendQue.size() > 0){
        pending = true;
    }
    sendQue.push(std::make_shared<msgNode>(msg, maxlen));
    if(pending){
        return;
    }

    boost::asio::async_write(sock, asio::buffer(msg, maxlen),
    std::bind(&Session::handleWrite, this, std::placeholders::_1, sharedSelf()));

}

void Session::close(){
    sock.close();
    bclose = true;
}

std::shared_ptr<Session> Session::sharedSelf(){
    return  shared_from_this();
}