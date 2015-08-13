#include <fstream>
#include <cassert>
#include <iostream>

#include "json/json.h"

using namespace std;
 
int main()
{
    ifstream ifs;
    ifs.open("array.json");
    assert(ifs.is_open());
 
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
        return -1;
    }
 
    std::string name;
    int age;
    int size = root.size();
    for (int i=0; i<size; ++i)
    {
        name = root[i]["name"].asString();
        age = root[i]["age"].asInt();
 
        std::cout<<name<<" "<<age<<std::endl;
    }
 
    return 0;
}