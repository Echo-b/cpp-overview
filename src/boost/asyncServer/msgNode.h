#ifndef __MSGNODE__
#define __MSGNODE__

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <memory>

#define HEAD_LENGTH 2

class msgNode
{
    friend class Session;
public:
    char* data;
    int curLen;
    int totalLen;
public:
    // write node
    msgNode(const char* msg, int maxlen): totalLen(maxlen + HEAD_LENGTH), curLen(0){
        data = new char[totalLen + 1]();
        int maxLenHost = boost::asio::detail::socket_ops::host_to_network_short(maxlen);
        memcpy(data, &maxLenHost, HEAD_LENGTH);
        memcpy(data + HEAD_LENGTH, msg, maxlen);
        data[totalLen] = '\0';
        
    };
    //read node
    msgNode(int maxlen) : totalLen(maxlen), curLen(0){
        data = new char[totalLen + 1]();
    }

    void clear(){
        memset(data, 0, totalLen);
        curLen = 0;
    }

    ~msgNode(){
        delete[] data;
    }
};


#endif