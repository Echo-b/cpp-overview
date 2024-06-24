#include "logicSystem.h"
// #include "session.cpp"

logicSystem::logicSystem():bstop(false)
{
    registerCallBack();
    workerThread = std::thread(&logicSystem::dealMsg, this);
}

void logicSystem::dealMsg(){
    for(;;){
        std::unique_lock<std::mutex> locker(lmutex);
        while(msgQue.empty() && !bstop){
            consume.wait(locker);
        }

        if(bstop){
            while(!msgQue.empty()){
                auto &msgnode = msgQue.front();
                std::cout << "recv msg id is" << msgnode->recvnode->getMsgId() << std::endl; 
                auto callBackIter = funCallBacks.find(msgnode->recvnode->getMsgId());
                if(callBackIter == funCallBacks.end()){
                    msgQue.pop();
                    continue;
                }
                break;
            }
        }

        auto &msgnode = msgQue.front();
        std::cout << "recv msg id is" << msgnode->recvnode->getMsgId() << std::endl; 
        auto callBackIter = funCallBacks.find(msgnode->recvnode->getMsgId());
        if(callBackIter == funCallBacks.end()){
            msgQue.pop();
            continue;
        }
        callBackIter->second(msgnode->session, msgnode->recvnode->getMsgId(), msgnode->recvnode->data);
        msgQue.pop();
    }
}

void logicSystem::registerCallBack(){
    funCallBacks[MSG_HELLO_WORD] = std::bind(&logicSystem::helloWoldCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}


void logicSystem::helloWoldCallBack(std::shared_ptr<Session> _session, short msgid, std::string msgdata){
    // parse json data
    Json::Reader read;
    Json::Value root;
    read.parse(msgdata, root);
    std::cout << "recv msgid is: " << root["id"].asInt() << " ,msg info is: " << root["data"].asString() << std::endl;
    root["data"] = "server has received msg, msg data is:" + root["data"].asString();
    std::string retStr = root.toStyledString();
    _session->send(retStr, root["id"].asInt());
}

void logicSystem::postMsgToQue(std::shared_ptr<logicNode> msg){
    std::unique_lock<std::mutex> locker(lmutex);
    msgQue.push(msg);

    if(msgQue.size() == 1){
        locker.unlock();
        consume.notify_one();
    }
}


logicSystem::~logicSystem()
{
    bstop = true;
    consume.notify_one();
    workerThread.join();
}