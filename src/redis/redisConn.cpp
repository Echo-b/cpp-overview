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

string redisConn::setCommand(string type, string key, string value)
{
    ostringstream oss;
    oss << type << " " << key << " " << value;
    log(oss.str());
    return oss.str();
}

bool redisConn::set(string key, string value)
{
    return execute(setCommand("SET", key, value)) != "";
}

string redisConn::get(string key)
{
    return execute(setCommand("GET", key));
}

bool redisConn::update(string key, string value)
{
    return execute(setCommand("SET", key, value)) != "";
}

bool redisConn::del(string key)
{
    return execute(setCommand("DEL", key)) != "";
}

string redisConn::execute(string command)
{
    string res = "";
    redisReply *r = (redisReply *)redisCommand(r_redis, command.c_str());
    if (r == nullptr)
    {
        freeReplyObject(r);
        redisFree(r_redis);
        return res;
    }
    if (r->type == REDIS_REPLY_STRING)
        res = r->str;
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