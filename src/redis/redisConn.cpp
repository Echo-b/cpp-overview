#include "redisConn.h"

redisConn::redisConn()
{
}

redisConn::~redisConn()
{
}

bool redisConn::connect(string ip, uint32_t port)
{
    r_redis = redisConnect(ip.c_str(), port);
    if (r_redis == nullptr || r_redis->err)
    {
        if (r_redis)
        {
            log(r_redis->errstr);
            redisFree(r_redis);
            return false;
        }
    }
    return true;
}

redisReply* redisConn::setCommand(string type, string key, string value)
{
    redisReply *r = nullptr;
    if (!value.empty()) {
        r = (redisReply *)redisCommand(r_redis, "SET %s %s", key.c_str(), value.c_str());
    }else{
        if(type == "GET")
            r = (redisReply *)redisCommand(r_redis, "GET %s", key.c_str(), value.c_str());
        else
            r = (redisReply *)redisCommand(r_redis, "DEL %s", key.c_str(), value.c_str());
    }
    return r;
}

bool redisConn::set(string key, string value)
{
    return execute("SET", key, value) != "";
}

string redisConn::get(string key)
{
    return execute("GET", key);
}

bool redisConn::update(string key, string value)
{
    string res = "";
    redisReply *r = (redisReply *)redisCommand(r_redis, "SET %s %s", key.c_str(), value.c_str());
    if (r == nullptr)
    {
        freeReplyObject(r);
        redisFree(r_redis);
        return false;
    }
    if (r->type == REDIS_REPLY_STRING)
        res = r->str;
    freeReplyObject(r);
    return true;
}

bool redisConn::del(string key)
{
    return execute("DEL", key) != "";
}

string redisConn::execute(string type, string key, string value)
{
    string res = "";
    redisReply *r = setCommand(type, key, value);
    if (r == nullptr)
    {
        freeReplyObject(r);
        redisFree(r_redis);
        return res;
    }
    if (r->type == REDIS_REPLY_STRING || (r->type == REDIS_REPLY_STATUS && std::string(r->str) == "OK"))
        res = std::string(r->str);
    freeReplyObject(r);
    return res;
}

void redisConn::log(string info, string parm)
{
    cout << info << parm << endl;
}

void redisConn::refreshAliveTime()
{
    r_aliveTime = chrono::steady_clock::now();
}

long long redisConn::getAliveTime()
{
    chrono::nanoseconds res = chrono::steady_clock::now() - r_aliveTime;
    chrono::milliseconds millsec = chrono::duration_cast<chrono::milliseconds>(res);
    return millsec.count();
}