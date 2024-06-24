#ifndef __LOGICSYSTEM__
#define __LOGICSYSTEM__

#include <functional>
#include <map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <jsoncpp/json/json.h>
#include <thread>
#include <iostream>
#include "singleTon.h"
#include "session.h"

typedef std::function<void(std::shared_ptr<Session>, short msgid, std::string msgdata)> funCallBack;

class logicSystem:public singleTon<logicSystem>
{
    friend class singleTon<logicSystem>;
private:
    std::thread workerThread;
    std::queue<std::shared_ptr<logicNode>> msgQue;
    std::mutex lmutex;
    std::condition_variable consume;
    std::map<short, funCallBack> funCallBacks;
    bool bstop;

private:
    logicSystem();
    void dealMsg();
    void registerCallBack();
    void helloWoldCallBack(std::shared_ptr<Session> _session, short msgid, std::string msgdata);
    
public:
    ~logicSystem();
    void postMsgToQue(std::shared_ptr<logicNode> msg);
};



#endif