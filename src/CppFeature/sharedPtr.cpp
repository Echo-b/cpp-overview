#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <list>

void log(std::string info, std::string param = "")
{
    std::cout << info << param << std::endl;
}

void delFunc(std::string *p)
{
    if (p != nullptr)
    {
        delete p;
        p = nullptr;
    }
    log("self delete");
}

void sharedPtrUsing()
{
    auto psint2 = std::make_shared<int>(5);
    if (psint2 != nullptr)
    {
        log(std::to_string(*psint2));
        int *p = psint2.get();
        log(std::to_string(*p));
    }
    auto psstr2 = std::make_shared<std::string>("hello world");
    if (psstr2 != nullptr && !psstr2->empty())
        log(*psstr2);
    auto psint3 = psint2;
    log("the count times is: ", std::to_string(psint2.use_count()));
    log("the count times is: ", std::to_string(psint3.use_count()));
    psint3 = std::make_shared<int>(110);
    log("the count times is: ", std::to_string(psint2.use_count()));
    log("the count times is: ", std::to_string(psint3.use_count()));

    auto psint4 = std::shared_ptr<std::string>(psstr2);
    log("the count time is: ", std::to_string(psint4.use_count()));
    log("the count time is: ", std::to_string(psstr2.use_count()));

    std::shared_ptr<std::string> psstr3(new std::string("hello world echobai"), delFunc);
}

void uniquePtrUsing()
{
}

void weakPtrUsing()
{
}

void lambdaUsing()
{

    int age = 16;
    std::string name = "echopi";
    int score = 100;

    auto func = [&name, &age](std::string lname)
    {
        std::cout << "hello " << lname << std::endl;
        age += 1;
        name = "Echo";
    };
    func("echobai");
    log("the age value is: ", std::to_string(age));
    log("the name value is:", name);
}

std::vector<std::function<void(std::string)>> vec_Funcs;
void lambdaCaputre()
{
    int age = 33;
    std::string name = "zack";
    int score = 100;
    std::string job = "softengineer";
    vec_Funcs.push_back(
        [name, age](std::string _name)
        {
            log("this is value catch");
            std::cout << "age is: " << age << " name is: " << name << " self name: " << _name << std::endl;
        });
    vec_Funcs.push_back(
        [&name, &age](std::string _name)
        {
            log("this is reference catch");
            std::cout << "age is: " << age << " name is: " << name << " self name: " << _name << std::endl;
        });
}

void testLambdaCapture()
{
    for (auto f : vec_Funcs)
    {
        f("zack");
    }
}

void valueAndRefCapture()
{
    int age = 33;
    std::string name = "zack";
    int score = 100;
    std::string job = "softengineer";

    [=, &name]()
    {
        std::cout << "age is: " << age << " name is: " << name << std::endl;
        name = "EchoBai";
    }();

    [&, name]()
    {
        std::cout << "age is: " << age << " name is: " << name << std::endl;
    }();
}

void testFunc(std::string _name, int _age)
{
    log("the name value is: ", _name);
    log("the age value is: ", std::to_string(_age));
}

void funcUsing()
{
    std::list<std::function<void(std::string)>> listFuncs;
    listFuncs.push_back([](std::string _name)
                        { log("lambda function list insert call"); });
    auto fun1 = std::bind(testFunc, std::placeholders::_1, 22);
    fun1("EchoBai");
}

class BindTestClass
{
public:
    BindTestClass(int _num, std::string _name) : num(_num), name(_name){};
    static void StaticFunc(const std::string &str, int age);
    void memberFunc(const std::string &job, int score);

public:
    int num;
    std::string name;
};

void BindTestClass::StaticFunc(const std::string &str, int age)
{
    log("this is a static function");
    log("the name is: ", str);
    log("the age is:", std::to_string(age));
}

void BindTestClass::memberFunc(const std::string &job, int score)
{
    log("this is a member function");
    log("the name is: ", name);
    log("the age is:", std::to_string(num));
    log("the job is: ", job);
    log("the score is:", std::to_string(score));
}

void bindStaticFunc()
{
    auto staticBind = std::bind(&BindTestClass::StaticFunc, std::placeholders::_1, 33);
    staticBind("echobai");
}

void bindMemberFunc()
{
    BindTestClass btc(22, "echobai");
    auto memberBind = std::bind(&BindTestClass::memberFunc, &btc, std::placeholders::_1, std::placeholders::_2);
    memberBind("coder", 100);

    auto memberBind2 = std::bind(&BindTestClass::memberFunc, std::placeholders::_3, std::placeholders::_1, std::placeholders::_2);
    memberBind2("coder", 99, &btc);

    auto numBind = std::bind(&BindTestClass::num, std::placeholders::_1);
    log("the bind num value is:", std::to_string(numBind(btc)));
}

void functionalWithBind()
{
    BindTestClass btc(22, "echobai");
    std::function<void(std::string, int)> memberBind = std::bind(&BindTestClass::memberFunc, &btc, std::placeholders::_1, std::placeholders::_2);
    memberBind("coder", 100);

    std::function<void(std::string, int, BindTestClass *btc)> memberBind2 = std::bind(&BindTestClass::memberFunc, std::placeholders::_3, std::placeholders::_1, std::placeholders::_2);
    memberBind2("coder", 99, &btc);

    std::function<int(const BindTestClass &)> numBind = std::bind(&BindTestClass::num, std::placeholders::_1);
    log("the bind num value is:", std::to_string(numBind(btc)));
}

int main()
{
    // sharedPtrUsing();
    // lambdaUsing();
    // lambdaCaputre();
    // testLambdaCapture();
    // valueAndRefCapture();
    // funcUsing();
    // bindStaticFunc();
    // bindMemberFunc();
    functionalWithBind();
    return 0;
}