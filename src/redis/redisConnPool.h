#ifndef _REDISCONNPOOL_
#define _REDISCONNPOOL_

#include <iostream>
#include <queue>
#include "redisConn.h"
#include "redisConn.cpp"
#include <thread>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <chrono>
#include <condition_variable>

class redisConnPool
{
private:
    std::queue<redisConn *> r_redisConnQ;
    string ip;
    uint32_t port;
    int r_maxSize;
    int r_minSize;
    int r_timeout;
    int r_maxIdleTime;
    mutex r_mutex;
    condition_variable r_cond;

private:
    redisConnPool();
    bool parseJsonFile();
    void produceConn();
    void recycleConn();
    void createConn();

public:
    static redisConnPool *getRedisConnPool();
    redisConnPool(const redisConnPool &obj) = delete;
    redisConnPool &operator=(const redisConnPool &obj) = delete;
    shared_ptr<redisConn> getRedisConn();
    ~redisConnPool();
};

#endif