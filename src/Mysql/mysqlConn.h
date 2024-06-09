#pragma once
#ifndef _MYSQLCONN_
#define _MYSQLCONN_ 

#include <iostream>
#include <memory>
#include <string>
#include <mysql/mysql.h>
#include <chrono>

using namespace std;

class mysqlConn
{
private:
    MYSQL* m_conn = nullptr;
    MYSQL_RES* m_res = nullptr;
    MYSQL_ROW m_row = nullptr;
    chrono::steady_clock::time_point m_aliveTime;

public:
    mysqlConn();
    bool connect(string user, string pass, string dbName, string ip, u_int32_t port = 3306);
    bool update(string sql);
    bool query(string sql);
    bool next();
    string getvalue(int idx);
    bool transaction();
    bool commit();
    bool rollback();
    void refreshAliveTime();
    long long getAliveTime();
    ~mysqlConn();

private:
    void freespace();
};

#endif