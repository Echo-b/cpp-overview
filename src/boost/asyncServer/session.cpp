#include "session.h"
#include "server.h"
// #include "server.cpp"

Session::Session(asio::io_context &ioc, Server *_server): sock(ioc), server(_server), bclose(false), bHeadParse(false){
    boost::uuids::uuid auuid = boost::uuids::random_generator()();
    uuid = boost::uuids::to_string(auuid);
    recvHeadNode = std::make_shared<msgNode>(HEAD_LENGTH);
}

Session::~Session(){
    std::cout << "destruct session" << std::endl;
}

void Session::printRecvData(char* data, int length) {
    std::stringstream ss;
    std::string result = "0x";
    for (int i = 0; i < length; i++) {
        std::string hexstr;
        ss << std::hex << std::setw(2) << std::setfill('0') << int(data[i]) << std::endl;
        ss >> hexstr;
        result += hexstr;
    }
    std::cout << "receive raw data is : " << result << std::endl;;
}

void Session::handleRead(const boost::system::error_code &ec, std::size_t bytesTransfreed, std::shared_ptr<Session> shared_self){
    if(!ec){
        printRecvData(data, bytesTransfreed);
        std::chrono::milliseconds dura(20);
        std::this_thread::sleep_for(dura);

        // already recv len
        int copyLen = 0;
        while (bytesTransfreed > 0){
            if(!bHeadParse) {
                if(bytesTransfreed + recvHeadNode->curLen < HEAD_LENGTH){
                    memcpy(recvHeadNode->data + recvHeadNode->curLen, data + copyLen, bytesTransfreed);
                    recvHeadNode->curLen += bytesTransfreed;
                    memset(data, 0, MAX_LENGTH);
                    sock.async_read_some(asio::buffer(data, MAX_LENGTH), 
                    std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                    return;
                }

                int headRemain = HEAD_LENGTH - recvHeadNode->curLen;
                memcpy(recvHeadNode->data + recvHeadNode->curLen, data + copyLen, headRemain);
                copyLen += headRemain;
                bytesTransfreed -= headRemain;

                // get head data
                short dataLen = 0;
                memcpy(&dataLen, recvHeadNode->data, HEAD_LENGTH);
                dataLen = boost::asio::detail::socket_ops::network_to_host_short(dataLen);
                std::cout << "data len:" << dataLen << std::endl;
                if(dataLen > MAX_LENGTH){
                    std::cout << "invalid data length is: " << dataLen << std::endl;
                    server->clearSession(uuid);
                    return;
                }

                recvMsgNode = std::make_shared<msgNode>(dataLen);
                if(bytesTransfreed < dataLen){
                    memcpy(recvMsgNode->data + recvMsgNode->curLen, data + copyLen, bytesTransfreed);
                    recvMsgNode->curLen += bytesTransfreed;
                    memset(data, 0, MAX_LENGTH);
                    sock.async_read_some(asio::buffer(data, MAX_LENGTH), 
                    std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                    bHeadParse = true;
                    return;
                }

                memcpy(recvMsgNode->data + recvMsgNode->curLen, data + copyLen, dataLen);
                recvMsgNode->curLen += dataLen;
                copyLen += dataLen;
                bytesTransfreed -= dataLen;
                recvMsgNode->data[recvMsgNode->totalLen] = '\0';
                std::cout << "server receive data is:"  << recvMsgNode->data << std::endl;
                send(recvMsgNode->data, recvMsgNode->totalLen);
                bHeadParse = false;
                recvHeadNode->clear();
                
                if(bytesTransfreed <= 0){
                    memset(data, 0, MAX_LENGTH);
                    sock.async_read_some(asio::buffer(data, MAX_LENGTH), 
                    std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                    return;
                }
                continue;
            }

            int remainMsg = recvMsgNode->totalLen - recvMsgNode->curLen;
            if(bytesTransfreed < remainMsg){
                memcpy(recvMsgNode->data + recvMsgNode->curLen, data + copyLen, bytesTransfreed);
                recvMsgNode->curLen += bytesTransfreed;
                memset(data, 0, MAX_LENGTH);
                sock.async_read_some(asio::buffer(data, MAX_LENGTH), 
                std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                return;
            }

            memcpy(recvMsgNode->data + recvMsgNode->curLen, data + copyLen, remainMsg);
            recvMsgNode->curLen += remainMsg;
            bytesTransfreed -= remainMsg;
            copyLen += remainMsg;
            recvMsgNode->data[recvMsgNode->totalLen] = '\0';
            std::cout << "server receive data is:"  << recvMsgNode->data << std::endl;

            send(recvMsgNode->data, recvMsgNode->totalLen);
            bHeadParse = false;
            recvHeadNode->clear();

            if(bytesTransfreed <= 0){
                memset(data, 0, MAX_LENGTH);
                sock.async_read_some(asio::buffer(data, MAX_LENGTH), 
                std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                return;
            }
            continue;
        }
    }else{
        std::cout << "handle read failed error is:" << ec.what() << std::endl;
        close();
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
    std::unique_lock<std::mutex> sendMutex(smutex);
    int qSize = sendQue.size();
    if(qSize > MAX_SENDQUE){
        std::cout << "session: " << uuid << "send que fulled size is :" << MAX_SENDQUE << std::endl;
        return;
    }
    sendQue.push(std::make_shared<msgNode>(msg, maxlen));
    if(qSize > 0){
        return;
    }
    auto &msgnode = sendQue.front(); 
    boost::asio::async_write(sock, asio::buffer(msgnode->data, msgnode->totalLen),
    std::bind(&Session::handleWrite, this, std::placeholders::_1, sharedSelf()));

}

void Session::close(){
    sock.close();
    bclose = true;
}

std::shared_ptr<Session> Session::sharedSelf(){
    return  shared_from_this();
}