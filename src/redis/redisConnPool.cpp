#include "redisConnPool.h"

redisConnPool::redisConnPool()
{
    for (int i = 0; i < r_minSize; ++i)
        createConn();
    thread producer(&redisConnPool::produceConn, this);
    thread recycle(&redisConnPool::recycleConn, this);
    producer.detach();
    recycle.detach();
}

redisConnPool::~redisConnPool()
{
    while (!r_redisConnQ.empty())
    {
        redisConn *conn = r_redisConnQ.front();
        r_redisConnQ.pop();
        delete conn;
    }
}

redisConnPool *redisConnPool::getRedisConnPool()
{
    static redisConnPool r_pool;
    return &r_pool;
}

shared_ptr<redisConn> redisConnPool::getRedisConn()
{
    unique_lock<mutex> locker(r_mutex);
    while(r_redisConnQ.empty()){
        if(cv_status::timeout == r_cond.wait_for(locker, chrono::milliseconds(r_timeout))){
            if(r_redisConnQ.empty())
                continue;
        }
    }
    shared_ptr<redisConn> connprt(r_redisConnQ.front(), [this](redisConn* conn){
        unique_lock<mutex> locker(r_mutex);
        conn->refreshAliveTime();
        r_redisConnQ.push(conn);

    });
    r_redisConnQ.pop();
    r_cond.notify_all();
    return connprt; 
}

bool redisConnPool::parseJsonFile()
{
    ifstream ifs("reconf.json");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs,root);
    if(root.isObject()){
        ip = root["ip"].asString();
        port = root["port"].asUInt();
        r_maxSize = root["maxSize"].asInt();
        r_minSize= root["minSize"].asInt();
        r_timeout= root["timeout"].asInt();
        r_maxIdleTime= root["maxIdleTime"].asInt();
        return true;
    }
    return false;
}

void redisConnPool::produceConn()
{
    while (true)
    {
        unique_lock<mutex> locker(r_mutex);
        while(r_redisConnQ.size() >= r_minSize){
            r_cond.wait(locker);
        }
        createConn();
        r_cond.notify_all();
    }
    
}

void redisConnPool::recycleConn()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(500));
        unique_lock<mutex> locker(r_mutex);
        while(r_redisConnQ.size() > r_minSize){
            redisConn* conn = r_redisConnQ.front();
            if(conn->getAliveTime() >= r_maxIdleTime){
                r_redisConnQ.pop();
                delete conn;
            }else break;
        }
    }
    
}

void redisConnPool::createConn()
{
    redisConn *conn = new redisConn();
    conn->connect(ip, port);
    conn->refreshAliveTime();
    r_redisConnQ.push(conn);
}