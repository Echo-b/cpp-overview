#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <string.h>


void log(std::string s, std::string parm = ""){
    std::cout << s << parm << std::endl;
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
    redisReply* r = (redisReply*) redisCommand(c, commamd1);
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

    const char* command2 = "strlen test1"; 
    r = (redisReply*)redisCommand(c, command2); 
    if ( r->type != REDIS_REPLY_INTEGER) 
    { 
        printf("Failed to execute command[%s]\n",command2); 
        freeReplyObject(r); 
        redisFree(c); 
        return; 
    } 

    int length = r->integer;
    freeReplyObject(r);
    log("the length of test1 is:", std::to_string(length));
    log("Succeed to execute command", command2);

}

int main(){
    test();
    return 0;
}