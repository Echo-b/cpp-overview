#include "mysqlConnPool.cpp"
#include "mysqlConnPool.h"

void opt1(int begin, int end){
    for(int i = begin; i < end; ++i){
        mysqlConn conn;
        conn.connect("root", "##", "test", "localhost");
        char sql[1024] = {0};
        sprintf(sql, "insert into dept values(%d, 'peng', 'bed')", i);
        // string sql = "insert into dept values(3, 'peng', 'bed')";
        conn.update(sql);
    }
}

void opt2(mysqlConnPool* pool, int begin, int end){
    for(int i = begin; i < end; ++i){
        shared_ptr<mysqlConn> conn = pool->getMysqlConn();
        // conn->connect("root", "##", "test", "localhost");
        char sql[1024] = {0};
        sprintf(sql, "insert into dept values(%d, 'peng', 'bed')", i);
        // string sql = "insert into dept values(3, 'peng', 'bed')";
        conn->update(sql);
    }

}

void test(){
#if 0
    // without conn pool signal thread time:3317621907 ns 3317 ms
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    opt1(0, 5000);
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    std::cout << "without conn pool signal thread time:" << length.count() << " ns "
    << length.count() / 1000000 << " ms" << std::endl;
#else
    // with conn pool signal thread time:2090041978 ns 2090 ms
    mysqlConnPool* pool = mysqlConnPool::getmysqlConnPool();
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    opt2(pool, 0, 5000);
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    std::cout << "with conn pool signal thread time:" << length.count() << " ns "
    << length.count() / 1000000 << " ms" << std::endl;
#endif
}

void test1(){
#if 0
    //without conn pool mult thread time:1834707784 ns 1834 ms
    mysqlConn conn;
    conn.connect("root", "##", "test", "localhost");
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    thread t1(opt1, 0, 1000);
    thread t2(opt1, 1000, 2000);
    thread t3(opt1, 2000, 3000);
    thread t4(opt1, 3000, 4000);
    thread t5(opt1, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    std::cout << "without conn pool mult thread time:" << length.count() << " ns "
    << length.count() / 1000000 << " ms" << std::endl;
#else
    // with conn pool mult thread time:922543450 ns 922 ms
    mysqlConnPool* pool = mysqlConnPool::getmysqlConnPool();
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    thread t1(opt2, pool, 0, 1000);
    thread t2(opt2, pool, 1000, 2000);
    thread t3(opt2, pool, 2000, 3000);
    thread t4(opt2, pool, 3000, 4000);
    thread t5(opt2, pool, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    std::cout << "with conn pool mult thread time:" << length.count() << " ns "
    << length.count() / 1000000 << " ms" << std::endl;
#endif
}



void query(){
    mysqlConn conn;
    conn.connect("root", "##", "test", "localhost");
    string sql = "insert into dept values(3, 'peng', 'bed')";
    int ret = conn.update(sql);
    std::cout << "ret = " << ret << std::endl;

    sql = "select * from dept";
    conn.query(sql);
    while(conn.next()){
        std::cout << conn.getvalue(0) << ",";
        std::cout << conn.getvalue(1) << ",";
        std::cout << conn.getvalue(2) << std::endl;
    }
}

int main(){
    test();
    return 0;
}