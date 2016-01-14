#include <msgpack.hpp>
#include <vector>
#include <string>
const int SIZE = 10;

class myclass {
    public:
        myclass()
        {
            m_id = 0x123456;
            m_str = "0x1234567890";

            for (int i = 0; i < SIZE; i++)
            {
                m_vec.push_back(m_str);
            }
        }

        int id()
        {
            return m_id;
        }

    protected:
        int m_id;
        std::string m_str;
        std::vector<std::string> m_vec;
    public:
        MSGPACK_DEFINE(m_id, m_str, m_vec);
};

int main(int argc, const char *argv[]) 
{
    msgpack::sbuffer sbuf;
    msgpack::unpacked msg;

    myclass cls;
    std::vector<myclass> vec;
    vec.push_back(cls);

    for (int i = 0; i < 1000000; i++)
    {
        msgpack::pack(sbuf, vec);
        sbuf.data();

        msgpack::unpack(&msg, sbuf.data(), sbuf.size());
        msgpack::object obj = msg.get();
        std::vector<myclass> rvec;
        
        
        obj.convert(&rvec);

        if (rvec[0].id() != 0x123456)
        {
            fprintf(stderr, "Error\n");
            exit(0);
        }
    }

    return 0;
}
