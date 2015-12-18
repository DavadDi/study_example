#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string>
#include "json/json.h"

////  libcurl reader callback function
static int reader_callback(char *data, size_t size, size_t nitems, std::string *read_data)
{
    if (read_data == NULL)
    {
        return 0;
    }

    read_data->append(data, size*nitems);

    return size * nitems;

}

static size_t header_callback(char *data, size_t size, size_t nitems, std::string  *read_data)
{
    if (read_data == NULL)
    {
        return 0;
    }

    read_data->append(data, size*nitems);
    return size * nitems;
}

static void print_value_tree( FILE *fout, Json::Value &value, const std::string &path = "."  )
{
    switch (value.type())
    {
        case Json::nullValue:
            fprintf( fout, "%s=null\n", path.c_str()  );
            break;

        case Json::intValue:
            fprintf( fout, "%s=%d\n", path.c_str(), value.asInt()  );
            break;

        case Json::uintValue:
            fprintf( fout, "%s=%u\n", path.c_str(), value.asUInt()  );
            break;

        case Json::realValue:
            fprintf( fout, "%s=%.16g\n", path.c_str(), value.asDouble()  );
            break;

        case Json::stringValue:
            fprintf( fout, "%s=\"%s\"\n", path.c_str(), value.asString().c_str()  );
            break;

        case Json::booleanValue:
            fprintf( fout, "%s=%s\n", path.c_str(), value.asBool() ? "true" : "false"  );
            break;

        case Json::arrayValue:
            {
                fprintf( fout, "%s=[] arrayValue \n", path.c_str()  );
                int size = value.size();
                for ( int index =0; index < size; ++index  )
                {
                    static char buffer[16];
                    sprintf( buffer, "[%d]", index  );
                    print_value_tree( fout, value[index], path + buffer);

                }

            }
            break;

        case Json::objectValue:
            {
                fprintf( fout, "%s={} objectValue \n", path.c_str()  );
                Json::Value::Members members( value.getMemberNames()  );
                std::sort( members.begin(), members.end()  );
                std::string suffix = *(path.end()-1) == '.' ? "" : ".";
                for ( Json::Value::Members::iterator it = members.begin(); it != members.end();++it)
                {
                    const std::string &name = *it;
                    print_value_tree( fout, value[name], path + suffix + name  );

                }

            }
            break;

        default:
            break;
    }
}


int main(int argc, char *argv[])
{
    CURL *curl = NULL;
    CURLcode res;
    if(argc!=2)
    {
        printf("Usage : %s <url>\n", argv[0]);
        exit(1);
    }

    const char *url = argv[1];

    static std::string str_body;
    static std::string str_header;

    curl = curl_easy_init();
    if( curl == NULL)
    {
        fprintf(stderr, "curl_easy_init() failed\n");
        exit(1);
    }


    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, reader_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_body);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &str_header);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    // curl_easy_setopt(curl, CURLOPT_HEADER, 1);

    res = curl_easy_perform(curl);

    long respcode = 0;
    curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE, &respcode);

    fprintf(stderr, "http code: %ld\n", respcode);

    curl_easy_cleanup(curl);
    /*
       JArray = root["key_array"];
       for ( unsigned int i = 0; i < JArray.size(); i++  )
       {
       cout << "JSON array values: " << JArray[i].asString() << endl;
       }
       */

    Json::Reader json_reader;
    Json::Value json_value;

    fprintf(stderr, "Header: %s\n", str_header.c_str());
    fprintf(stderr, "--------------------------------------\n");

    try
    {
        if (json_reader.parse(str_body, json_value))
        {
            fprintf(stderr, "GET>>\n%s\n", json_value.toStyledString().c_str());
        }

        print_value_tree( stderr, json_value);
    }
    catch (...)
    {
        fprintf(stderr, "Got exception here for json parse\n");
    }

    return 0;

}
