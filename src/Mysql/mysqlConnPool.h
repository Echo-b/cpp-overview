#pragma once
#ifndef _MYSQLCONNPOOL_
#define _MYSQLCONNPOOL_ 

#include <iostream>
#include <memory>
#include <string>
#include "mysqlConn.h"
#include "mysqlConn.cpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <thread>
#include <chrono>

using namespace std;

class mysqlConnPool
{
private:
    mysqlConnPool();
    bool parseJsonFile();
    void produceConn();
    void recycleConn();
    void createConn();
public:
    static mysqlConnPool* getmysqlConnPool();
    mysqlConnPool(const mysqlConnPool &obj) = delete;
    mysqlConnPool& operator=(const mysqlConnPool &obj) = delete;
    shared_ptr<mysqlConn> getMysqlConn();
    ~mysqlConnPool();

private:
    queue<mysqlConn*> m_connPoolQ;
    string m_ip;
    string m_user;
    string m_pass;
    string m_dbName;
    uint32_t m_port;
    int m_maxSize;
    int m_minSize;
    int m_timeout;
    int m_maxIdleTime;
    mutex m_mutex;
    condition_variable m_cond;
};

#endif