#ifndef _REDISCONN_
#define _REDISCONN_

#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <chrono>

using namespace std;

class redisConn
{
private:
    redisContext *r_redis = nullptr;
    chrono::steady_clock::time_point r_aliveTime;

public:
    redisConn();
    bool connect(string ip, uint32_t port);
    string get(string key);
    string execute(string command);
    bool set(string key, string value);
    bool update(string key, string value);
    bool del(string key);
    void log(string info, string parm = "");
    string setCommand(string type, string key, string value = "");
    void refreshAliveTime();
    long long getAliveTime();
    ~redisConn();
};

#endif