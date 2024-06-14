#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

void log(std::string info, std::string param = "")
{
    std::cout << info << param << std::endl;
}

class StrBlob
{
public:
    typedef vector<string>::size_type size_type;
    StrBlob();
    StrBlob(const initializer_list<string> &li);
    StrBlob(const StrBlob &obj);
    StrBlob &operator=(const StrBlob &obj);
    size_type size() const { return data->size(); }
    bool empty() { return data->empty(); }
    void push_back(const string &s) { data->push_back(s); }
    void pop_back();
    string &front();
    string &back();
    void printCnt();

private:
    shared_ptr<vector<string>> data;
    void check(size_type idx, const string &msg) const;
    string badvalue = "error";
};

StrBlob::StrBlob()
{
    data = make_shared<vector<string>>();
}

StrBlob::StrBlob(const StrBlob &obj)
{
    data = obj.data;
}

StrBlob::StrBlob(const initializer_list<string> &li)
{
    data = make_shared<vector<string>>(li);
}

StrBlob &StrBlob::operator=(const StrBlob &obj)
{
    if (&obj != this)
    {
        this->data = obj.data;
    }
    return *this;
}

void StrBlob::check(size_type idx, const string &msg) const
{
    if (idx >= data->size())
    {
        throw out_of_range(msg);
    }
}

string &StrBlob::front()
{
    // check(0, "");
    check(0, "front on empty StrBlob");
    return data->front();
}

string &StrBlob::back()
{
    check(0, "back on empty StrBlob");
    return data->back();
}

void StrBlob::pop_back()
{
    check(0, "pop on pop_back StrBlob");
    data->pop_back();
}

void StrBlob::printCnt()
{
    cout << "shared_ptr use_count is : " << data.use_count() << endl;
}

int main()
{
    StrBlob obj = StrBlob({"hello", "echobai", "world"});
    StrBlob obj2;
    try
    {
        auto strobj2 = obj2.front();
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << e.what() << '\n';
    }
    catch (...)
    {
        log("unknown execption");
    }
    obj2 = obj;

    auto objstr1 = obj.front();

    log("objstr1 value is:", objstr1);

    StrBlob obj3 = obj;
    auto objstr3 = obj3.back();

    log("objstr3 value is: ", objstr3);

    obj.printCnt();
    obj2.printCnt();
    obj3.printCnt();

    return 0;
}