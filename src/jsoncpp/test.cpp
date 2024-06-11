#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <cassert>

using namespace std;
using namespace Json;

void parseFromFile(){
    ifstream ifs;
    ifs.open("test.json");
    assert(ifs.is_open());

    Reader rd;
    Value root;
    if(!rd.parse(ifs, root,false)){
        cout << "parse error" << endl;
        return; 
    }
    string name = root["name"].asString();
    int salary = root["salary"].asInt();

    cout << "name: " << name << " salary: " << salary << endl;
    ifs.close();
}

void parseFromMemObject(){
    const string jsonData = R"({"name" : "test", "salary" : 100, "msg" : "test", "file" : [ "1.cpp", "2.c", "3.exe" ]})";
    
    Reader rd;
    Value root;
    if(!rd.parse(jsonData, root)){
        cout << "parse error" << endl;
        return;
    }
    string name = root["name"].asString();
    int salary = root["salary"].asInt();
    string msg = root["msg"].asString();
    cout << "name: " << name << " salary: " << salary;
    cout << " msg: " << msg << endl;
    Value files = root["file"];
    for(uint i = 0; i < files.size(); ++i){
        cout << files[i].asString() << " ";
    }
    cout << endl;
    
}

void buildJsonObj(){
    Value root;
    FastWriter writer;
    Value person;
    person["name"] = "test";
    person["age"] = 22;
    root.append(person);

    string jsonStr = writer.write(root);
    cout << "jsonstr: "  << jsonStr << endl;


}


int main(){
    parseFromFile();
    parseFromMemObject();
    buildJsonObj();
    return  0;
}