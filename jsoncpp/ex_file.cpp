#include <iostream>
#include <fstream>
#include <cassert>
#include "json/json.h"
 
using namespace std;

int main()
{
    ifstream ifs;
    ifs.open("testjson.json");
    assert(ifs.is_open());
 
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
        return -1;
    }
 
    std::string name = root["name"].asString();
    int age = root["age"].asInt();
 
    std::cout<<name<<std::endl;
    std::cout<<age<<std::endl;
 
    return 0;
}