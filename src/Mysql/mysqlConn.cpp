#include "mysqlConn.h"

mysqlConn::mysqlConn()
{
    m_conn = mysql_init(nullptr);
    mysql_set_character_set(m_conn, "utf8");
}

mysqlConn::~mysqlConn()
{
    if(m_conn != nullptr)
        mysql_close(m_conn);
    freespace();
}

bool mysqlConn::connect(string user, string pass, string dbName, string ip, u_int32_t port)
{
    MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), pass.c_str(), dbName.c_str(), port, NULL, 0);
    return ptr != nullptr;
}

bool mysqlConn::update(string sql)
{
    if(mysql_query(m_conn, sql.c_str()))
        return false;
    return true;
}

bool mysqlConn::query(string sql)
{
    freespace();
    if(mysql_query(m_conn, sql.c_str()))
        return false;
    m_res = mysql_store_result(m_conn);
    return true;
}

bool mysqlConn::next()
{
    if(m_res != nullptr){
        m_row = mysql_fetch_row(m_res);
        if(m_row != nullptr) 
            return true;
    }
    return false;
}

string mysqlConn::getvalue(int idx)
{
    int rowNum = mysql_num_fields(m_res);
    if(idx >= rowNum || idx < 0)
        return string();
    char* value = m_row[idx];
    int len = mysql_fetch_lengths(m_res)[idx];
    return string(value, len);
    
}

bool mysqlConn::transaction()
{
    return mysql_autocommit(m_conn, false);
}

bool mysqlConn::commit()
{
    return mysql_commit(m_conn);
}

bool mysqlConn::rollback()
{
    return mysql_rollback(m_conn);
}

void mysqlConn::freespace(){
    if(m_res){
        mysql_free_result(m_res);
        m_res = nullptr;
    }
}

void mysqlConn::refreshAliveTime(){
    m_aliveTime = chrono::steady_clock::now();
}

long long mysqlConn::getAliveTime(){
    chrono::nanoseconds res = chrono::steady_clock::now() - m_aliveTime;
    chrono::milliseconds millsec = chrono::duration_cast<chrono::milliseconds>(res);
    return millsec.count();
}