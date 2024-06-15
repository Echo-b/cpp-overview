#ifndef __SESSION__
#define __SESSION__

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <string>
#include <queue>
#include "msgNode.cpp"

// using namespace std;
using namespace boost;

class Session
{
private:
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::shared_ptr<msgNode> sendNode;
    std::queue<std::shared_ptr<msgNode>> sendQueue;
    std::shared_ptr<msgNode> recvNode;
    bool recvPending;
    bool sendPending;
    
public:
    Session(std::shared_ptr<asio::ip::tcp::socket> &sock);
    void connect(const asio::ip::tcp::endpoint &ep);
    void writeCallBackErr(const boost::system::error_code &ec, std::size_t bytesTransferred, std::shared_ptr<msgNode> msgNode);
    void writeToSocketErr(const std::string &buf);
    void writeCallBack(const boost::system::error_code &ec, std::size_t byteTransferred);
    void writeToSocket(const std::string &buf);
    void writeAllCallBack(const boost::system::error_code &ec, std::size_t byteTransferred);
    void writeAllToSocket(const std::string &buf);
    void readFromSocket();
    void readCallBack(const boost::system::error_code &ec, std::size_t bytesTranseferred);
    void readAllFromSocket();
    void readAllCallBack(const boost::system::error_code &ec, std::size_t bytesTranseferred);
    ~Session();
};

Session::Session(std::shared_ptr<asio::ip::tcp::socket> &sock)
{
}

void Session::writeCallBackErr(const boost::system::error_code &ec, std::size_t bytesTransferred, std::shared_ptr<msgNode> msgNode){
    if(bytesTransferred + msgNode->curLen < msgNode->totalLen){
        // not finish send operator
        sendNode->curLen += bytesTransferred;
        this->sock->async_write_some(asio::buffer(sendNode->msg + sendNode->curLen, sendNode->totalLen - sendNode->curLen), 
        std::bind(&Session::writeCallBackErr, this, std::placeholders::_1, std::placeholders::_2, sendNode));

    }
}


void Session::writeToSocketErr(const std::string &buf){
    sendNode = std::make_shared<msgNode>(buf.c_str(), buf.length());
    this->sock->async_write_some(asio::buffer(sendNode->msg, sendNode->totalLen),
    std::bind(&Session::writeCallBackErr, this, std::placeholders::_1, std::placeholders::_2, sendNode));
}


void Session::writeCallBack(const boost::system::error_code &ec, std::size_t byteTransferred){
    if(ec.value() != 0){
        std::cout << "Error code is" << ec.value() << "msg is :" << ec.message() << std::endl;
        return;
    }
    auto & sendData =  sendQueue.front();
    sendData->curLen += byteTransferred;
    if(sendData->curLen < sendData->totalLen){
        this->sock->async_write_some(asio::buffer(sendData->msg + sendData->curLen, sendData->totalLen - sendData->curLen),
        std::bind(&Session::writeCallBack, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }
    sendQueue.pop();
    if(sendQueue.empty()){
        sendPending = false;
    }
    if(!sendQueue.empty()){
        auto &sendData = sendQueue.front();
        this->sock->async_write_some(asio::buffer(sendData->msg + sendData->curLen, sendData->totalLen - sendData->curLen),
        std::bind(&Session::writeCallBack, this, std::placeholders::_1, std::placeholders::_2));

    }
}

void Session::writeToSocket(const std::string &buf){
    sendQueue.emplace(new msgNode(buf.c_str(), buf.length()));
    if(sendPending)
        return;
    this->sock->async_write_some(asio::buffer(buf), std::bind(
        &Session::writeCallBack, this, std::placeholders::_1, std::placeholders::_2
    ));
    sendPending = true;
}

void Session::writeAllCallBack(const boost::system::error_code &ec, std::size_t byteTransferred){
    if(ec.value() != 0){
        std::cout << "Error code is" << ec.value() << "msg is :" << ec.message() << std::endl;
        return;
    }
    sendQueue.pop();

    if(sendQueue.empty()){
        sendPending = false;
    }
    if(!sendQueue.empty()){
        auto &sendData = sendQueue.front();
        this->sock->async_send(asio::buffer(sendData->msg + sendData->curLen, sendData->totalLen - sendData->curLen),
        std::bind(&Session::writeAllCallBack, this, std::placeholders::_1, std::placeholders::_2));

    }

}

void Session::writeAllToSocket(const std::string &buf){
    sendQueue.emplace(new msgNode(buf.c_str(), buf.length()));

    if(sendPending){
        return;
    }
    this->sock->async_send(asio::buffer(buf),
    std::bind(&Session::writeAllCallBack, this, std::placeholders::_1, std::placeholders::_2));
    sendPending = true;
}


void Session::connect(const asio::ip::tcp::endpoint &ep){
    sock->connect(ep);
}

void Session::readFromSocket(){
    if(recvPending) return;

    recvNode = std::make_shared<msgNode>(RECVSIZE);
    this->sock->async_read_some(asio::buffer(recvNode->msg, recvNode->totalLen),
    std::bind(&Session::readCallBack, this, std::placeholders::_1, std::placeholders::_2));
    recvPending = true;
}
void Session::readCallBack(const boost::system::error_code &ec, std::size_t bytesTranseferred){
    recvNode->curLen += bytesTranseferred;
    if(recvNode->curLen < recvNode->totalLen){
        this->sock->async_read_some(asio::buffer(recvNode->msg + recvNode->curLen, recvNode->totalLen - recvNode->curLen),
        std::bind(&Session::readCallBack, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }
    recvPending = false;
    recvNode = nullptr;
}

void Session::readAllFromSocket(){
    if(recvPending) return;

    recvNode = std::make_shared<msgNode>(RECVSIZE);
    this->sock->async_receive(asio::buffer(recvNode->msg, recvNode->totalLen),
    std::bind(&Session::readCallBack, this, std::placeholders::_1, std::placeholders::_2));
    recvPending = true;
}
void Session::readAllCallBack(const boost::system::error_code &ec, std::size_t bytesTranseferred){
    recvNode->curLen += bytesTranseferred;
    recvPending = false;
    recvNode = nullptr;
}

Session::~Session()
{
}

#endif