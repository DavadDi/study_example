// g++ -g -Wall -o test_json test_json.cpp -I./include -L./lib -ljson
#include <iostream>
#include <string>
#include "json/json.h"

using namespace std;

int main()
{
    {
        // one data
        cout << endl << "example 1:" << endl;
        string test = "{\"id\":1,\"name\":\"hello\"}";
        Json::Reader reader;
        Json::Value value;

        if (reader.parse(test, value))
        {
            int id = value["id"].asInt();
            string name = value["name"].asString();
            cout << id << " " << name << endl;
        }
        else
        {
            cout << "parse error" << endl;
        }
    }

    {
        // more data
        cout << endl << "example 2:" << endl;
        string test = "{\"array\":[{\"id\":1,\"name\":\"hello\"},{\"id\":2,\"name\":\"world\"}]}";
        Json::Reader reader;
        Json::Value value;

        if (reader.parse(test, value))
        {
            const Json::Value arrayObj = value["array"];
            for (int i = 0; i < arrayObj.size(); i++)
            {
                int id = arrayObj[i]["id"].asInt();
                string name = arrayObj[i]["name"].asString();
                cout << id << " " << name << endl;
            }
        }
        else
        {
            cout << "parse error" << endl;
        }
    }

    return 0;
}
