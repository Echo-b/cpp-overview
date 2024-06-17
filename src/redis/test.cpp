#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <string.h>
#include "redisConn.h"
#include "redisConn.cpp"

void log(std::string s, std::string parm = ""){
    std::cout << s << parm << std::endl;
}

void test2(){
    redisConn* conn = new redisConn;
    conn->connect("127.0.0.1", 6379);
    std::string infos = R"([{"description":"ask students some infos","fields":"["name", "gender","advice"]","id":"1","type":"ask"}])";
    bool ret = conn->set("infos", infos);
    log(to_string(ret));
    log(conn->get("infos"));
}

void test(){
    redisContext* c = redisConnect("127.0.0.1", 6379);
    if(c->err){
        redisFree(c);
        log("redis connect error");
        return;
    }
    log("connect successful");
    const char* commamd1 = "set test1 value1";
    std::string infos = R"([{"description":"ask students some infos","fields":"["name", "gender","advice"]","id":"1","type":"ask"}])";
    redisReply* r = (redisReply*) redisCommand(c, "set test1 %s", infos.c_str());
    if(r == NULL){
        log("execut command1 error");
        redisFree(c);
        return;
    }
    
    if(!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str, "OK") ) == 0){
        log("fail execute command:", commamd1);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    freeReplyObject(r);
    log("successful execute command:", commamd1);

    const char* command2 = "get test1"; 
    r = (redisReply*)redisCommand(c, command2); 
    if ( r->type != REDIS_REPLY_STRING) 
    { 
        printf("Failed to execute command[%s]\n",command2); 
        freeReplyObject(r); 
        redisFree(c); 
        return; 
    } 

    std::string msg = r->str;
    freeReplyObject(r);
    log("the content of test1 is:", msg);
    log("Succeed to execute command", command2);

}

int main(){
    test2();
    return 0;
}