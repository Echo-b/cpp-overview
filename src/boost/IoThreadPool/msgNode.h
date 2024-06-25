#ifndef __MSGNODE__
#define __MSGNODE__

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <boost/asio.hpp>
#include <jsoncpp/json/json.h>
#include "const.h"


class msgNode
{
    friend class Session;
public:
    char* data;
    short curLen;
    short totalLen;
public:
    msgNode(short maxlen) : totalLen(maxlen), curLen(0){
        data = new char[totalLen + 1]();
        data[totalLen] = '\0';
    }

    void clear(){
        memset(data, 0, totalLen);
        curLen = 0;
    }

    ~msgNode(){
        std::cout << "destruct msgNode" << std::endl;
        delete[] data;
    }
};

class recvNode:public msgNode {
private:
    short msgId;
public:
    recvNode(short maxlen, short msgid):msgNode(maxlen), msgId(msgid){

    }
    short getMsgId(){
        return msgId;
    }
};

class sendNode:public msgNode {
private:
    short msgId;
public:
    sendNode(const char* msg, short maxlen, short msgid):msgNode(maxlen + HEAD_TOTAL_LEN), msgId(msgid){
        short msgIdHost = boost::asio::detail::socket_ops::host_to_network_short(msgid);
        memcpy(data, &msgIdHost, HEAD_ID_LEN);
        short msgLenHost = boost::asio::detail::socket_ops::host_to_network_short(maxlen);
        memcpy(data + HEAD_ID_LEN, &msgLenHost, HEAD_DATA_LEN);
        memcpy(data + HEAD_ID_LEN + HEAD_DATA_LEN, msg, maxlen);
    }
    short getMsgId(){
        return msgId;
    }
};


#endif