#include <iostream>
#include <thread>
#include <string>

class backgroupTask
{
public:
    void operator()(std::string str)
    {
        std::cout << str << std::endl;
    }
};

void threadWork1(std::string str)
{
    std::cout << "string is :" << str << std::endl;
}

int main()
{
    std::thread t(threadWork1, "hello world");
    t.join();

    std::thread t1([](std::string str)
                   { std::cout << str << std::endl; }, "hello test");
    t1.join();

    std::thread t2{backgroupTask(), "hello t2"};
    t2.join();
    return 0;
}