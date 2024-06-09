#include "redisConn.h"
#include "redisConn.cpp"

void test()
{
    redisConn *redis = new redisConn();
    redis->connect("127.0.0.1", 6379);
    redis->set("echo", "bai");
    cout << redis->get("echo") << endl;
    redis->update("echo", "echopi");
    cout << redis->get("echo") << endl;
    redis->del("echo");
}

int main()
{
    test();
    return 0;
}