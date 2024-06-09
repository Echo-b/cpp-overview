#include "mysqlConnPool.h"

mysqlConnPool::mysqlConnPool(/* args */)
{
    if (!parseJsonFile())
        return;
    for (int i = 0; i < m_minSize; ++i)
        createConn();
    thread producer(&mysqlConnPool::produceConn, this);
    thread recycle(&mysqlConnPool::recycleConn, this);
    producer.detach();
    recycle.detach();
}

void mysqlConnPool::createConn()
{
    mysqlConn *conn = new mysqlConn();
    conn->connect(m_user, m_pass, m_dbName, m_ip, m_port);
    conn->refreshAliveTime();
    m_connPoolQ.push(conn);
}

void mysqlConnPool::produceConn()
{
    while (true)
    {
        unique_lock<mutex> locker(m_mutex);
        while (m_connPoolQ.size() >= m_minSize)
        {
            m_cond.wait(locker);
        }
        createConn();
        m_cond.notify_all(); // wake all thread but only consumer can work when while conditon is true
    }
}

void mysqlConnPool::recycleConn()
{
    while (true)
    {
        // every 1s check
        this_thread::sleep_for(chrono::milliseconds(500));
        unique_lock<mutex> locker(m_mutex);
        while (m_connPoolQ.size() > m_minSize)
        {
            mysqlConn *conn = m_connPoolQ.front();
            if (conn->getAliveTime() >= m_maxIdleTime)
            {
                m_connPoolQ.pop();
                delete conn;
            }
            else break;
        }
    }
}

mysqlConnPool::~mysqlConnPool()
{
    while(!m_connPoolQ.empty()){
        mysqlConn* conn = m_connPoolQ.front();
        m_connPoolQ.pop();
        delete conn;
    }
}

mysqlConnPool *mysqlConnPool::getmysqlConnPool()
{
    static mysqlConnPool pool;
    return &pool;
}

shared_ptr<mysqlConn> mysqlConnPool::getMysqlConn()
{
    unique_lock<mutex> locker(m_mutex);
    while (m_connPoolQ.empty())
    {
        if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout)))
        {
            if (m_connPoolQ.empty())
                continue;
        }
    }
    shared_ptr<mysqlConn> connptr(m_connPoolQ.front(), [this](mysqlConn *conn)
                                  {
        // m_mutex.lock();
        unique_lock<mutex> locker(m_mutex);
        conn->refreshAliveTime();
        m_connPoolQ.push(conn);
        // m_mutex.unlock(); 
        });
    m_connPoolQ.pop();
    m_cond.notify_all(); // wake all thread but only the producer can work
    return connptr;
}

bool mysqlConnPool::parseJsonFile()
{
    ifstream ifs("dbconf.json");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs, root);
    if (root.isObject())
    {
        m_ip = root["ip"].asString();
        m_user = root["user"].asString();
        m_pass = root["pass"].asString();
        m_dbName = root["dbName"].asString();
        m_port = root["port"].asUInt();
        m_maxSize = root["maxSize"].asInt();
        m_minSize = root["minSize"].asInt();
        m_timeout = root["timeout"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        return true;
    }
    return false;
}