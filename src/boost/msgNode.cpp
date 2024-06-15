#ifndef __MSGNODE__
#define __MSGNODE__

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <memory>

const int RECVSIZE = 1024;

class msgNode
{
public:
    char* msg;
    int curLen;
    int totalLen;
public:
    // write node
    msgNode(const char* _msg, int totalLen): totalLen(totalLen), curLen(0){
        msg = new char[totalLen];
        memcpy(msg, _msg, totalLen);
        
    };
    //read node
    msgNode(int totalLen) : totalLen(totalLen), curLen(0){
        msg = new char[totalLen];
    }
    ~msgNode();
};

msgNode::~msgNode()
{
    delete[] msg;
}


#endif