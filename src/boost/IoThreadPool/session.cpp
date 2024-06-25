#include "session.h"
#include "server.h"
#include "logicSystem.h"
#include "logicSystem.cpp"
// #include "server.cpp"

logicNode::logicNode(std::shared_ptr<Session> _session, std::shared_ptr<recvNode> _recvnode): session(_session), recvnode(_recvnode){

}

Session::Session(asio::io_context &ioc, Server *_server): sock(ioc), server(_server), bclose(false), bHeadParse(false), strand(ioc.get_executor()){
    boost::uuids::uuid auuid = boost::uuids::random_generator()();
    uuid = boost::uuids::to_string(auuid);
    recvHeadNode = std::make_shared<msgNode>(HEAD_TOTAL_LEN);
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

void Session::handleHeadRead(const boost::system::error_code &ec, std::size_t bytesTransfreed, std::shared_ptr<Session> shared_self){
    if(!ec){
        if(bytesTransfreed < HEAD_TOTAL_LEN){
            std::cout << "read head length error" << std::endl;
            close();
            server->clearSession(uuid);
            return;
        }

        short msgid = 0;
        memcpy(&msgid, recvHeadNode->data, HEAD_ID_LEN);
        msgid = boost::asio::detail::socket_ops::network_to_host_short(msgid);
        std::cout << "msgid is: " << msgid << std::endl;
        if(msgid > MAX_LENGTH){
            std::cout << "invalid msgid is:" << msgid << std::endl;
            server->clearSession(uuid);
            return;
        }

        // recv head 
        short dataLen = 0;
        memcpy(&dataLen, recvHeadNode->data + HEAD_ID_LEN, HEAD_DATA_LEN);
        dataLen = asio::detail::socket_ops::network_to_host_short(dataLen);
        std::cout << "data len is: " << dataLen << std::endl;

        if(dataLen > MAX_LENGTH){
            std::cout << "invalid data length is:" << dataLen << std::endl;
            server->clearSession(uuid);
            return;
        }

        recvMsgNode = std::make_shared<recvNode>(dataLen, msgid);
        boost::asio::async_read(sock, asio::buffer(recvMsgNode->data, recvMsgNode->totalLen),
        asio::bind_executor(strand, std::bind(&Session::handleMsgRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf()))
        );


    }else{
        std::cout << "handle read head failed error is:" << ec.what() << std::endl;
        close();
        server->clearSession(uuid);
    }
}

void Session::handleMsgRead(const boost::system::error_code &ec, std::size_t bytesTransfreed, std::shared_ptr<Session> shared_self){
    if(!ec){
        // test code
        // printRecvData(recvMsgNode->data, bytesTransfreed);
        // std::chrono::milliseconds dura(2000);
        // std::this_thread::sleep_for(dura);
        recvMsgNode->data[recvMsgNode->totalLen] = '\0';

        // parse json data
        // Json::Reader read;
        // Json::Value root;
        // read.parse(std::string(recvMsgNode->data, recvMsgNode->totalLen), root);
        // std::cout << "recv msgid is:" << root["id"].asInt() << " msg info is: " << root["data"].asString() << std::endl;
        // root["data"] = "server has received msg, msg data is:" + root["data"].asString();
        // std::string retStr = root.toStyledString();

        // send(retStr, root["id"].asInt());
        
        // post task to logicsystem
        logicSystem::getInstance()->postMsgToQue(std::make_shared<logicNode>(sharedSelf(), recvMsgNode));

        // again recv head
        recvHeadNode->clear();
        boost::asio::async_read(sock, asio::buffer(recvHeadNode->data, HEAD_TOTAL_LEN),
        asio::bind_executor(strand, std::bind(&Session::handleHeadRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf()))
        );

    }else{
        std::cout << "handle read msg failed error is:" << ec.what() << std::endl;
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
            asio::bind_executor(strand, std::bind(&Session::handleWrite, this, std::placeholders::_1, shared_self))
            );
        }
    }else{
        std::cout << "handle write failed error is:" << ec.what() << std::endl;
        server->clearSession(uuid);
    }
}

void Session::start(){
    recvHeadNode->clear();
    boost::asio::async_read(sock, asio::buffer(recvHeadNode->data, HEAD_TOTAL_LEN),
    asio::bind_executor(strand, std::bind(&Session::handleHeadRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf()))
    );
}

std::string &Session::getUuid(){
    return uuid;
}

void Session::send(char *msg, short maxlen, short msgid){
    std::unique_lock<std::mutex> sendMutex(smutex);
    int qSize = sendQue.size();
    if(qSize > MAX_SENDQUE){
        std::cout << "session: " << uuid << "send que fulled size is :" << MAX_SENDQUE << std::endl;
        return;
    }
    sendQue.push(std::make_shared<sendNode>(msg, maxlen, msgid));
    if(qSize > 0){
        return;
    }
    auto &msgnode = sendQue.front(); 
    boost::asio::async_write(sock, asio::buffer(msgnode->data, msgnode->totalLen),
    asio::bind_executor(strand, std::bind(&Session::handleWrite, this, std::placeholders::_1, sharedSelf()))
    );

}

void Session::send(std::string msg, short msgid){
    std::unique_lock<std::mutex> sendMutex(smutex);
    int qSize = sendQue.size();
    if(qSize > MAX_SENDQUE){
        std::cout << "session: " << uuid << "send que fulled size is :" << MAX_SENDQUE << std::endl;
        return;
    }
    sendQue.push(std::make_shared<sendNode>(msg.c_str(), msg.length(), msgid));
    if(qSize > 0){
        return;
    }
    auto &msgnode = sendQue.front(); 
    boost::asio::async_write(sock, asio::buffer(msgnode->data, msgnode->totalLen),
    asio::bind_executor(strand, std::bind(&Session::handleWrite, this, std::placeholders::_1, sharedSelf()))
    );

}

void Session::close(){
    sock.close();
    bclose = true;
}

std::shared_ptr<Session> Session::sharedSelf(){
    return  shared_from_this();
}