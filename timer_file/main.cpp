#include "thread_record.hpp"
#include <sstream>

template <class T>
std::string to_string(const T& t)
{
    std::ostringstream oss;
    oss << t << "\n";
    return oss.str();
}

int main()
{

    //STR_QUEUE str_queue;

    // start a recoder thread
    CRecordThread<std::string>  rec_th;
    CTimerFileInfo timer_file_info;

    timer_file_info.file_prefix("mysql");
    timer_file_info.file_suffix("log");
    timer_file_info.file_dir("./data");
    timer_file_info.file_max_size(1024*1024);
    timer_file_info.file_max_line(10000);
    timer_file_info.file_start_seq(0);

    if (rec_th.open( timer_file_info ) < 0)
    {
        return -1;
    }

    std::string str;
    for (int i = 0; i < 1000000; i++)
    {
        str = to_string(i);
        rec_th.put_msg(str);

        if (i %1000 == 0)
        {
             sleep_ms(5);
        }
    }

    rec_th.stop();

    return 0;
    /*
    ReaderWriterQueue<uint32_t> q(1024);

    auto reader_fun = [] (ReaderWriterQueue<uint32_t> &q)
            {
                fprintf(stderr, "Enter thread reader func\n");
                uint32_t element = 0;
                for (uint32_t j = 0; j < 1024;j ++)
                {
                    if (q.try_dequeue(element))
                    {
                        printf("Read %d\n", element);
                        ++j;
                    }

                    sleep(1);

                }

                fprintf(stderr, "Exit thread reader\n");
            };

   std::thread reader = std::thread(reader_fun, std::ref(q));


    auto writer_fun = [] (ReaderWriterQueue<uint32_t> &q)
    {
        fprintf(stderr, "Enter writer func");

        for(uint32_t i = 0; i < 1024; i++)
        {
            fprintf(stderr, "write data %d\n", i);
            q.enqueue(i);
            sleep(1);
        }

        fprintf(stderr, "Exit write func\n");
    };

    std::thread writer  = std::thread(writer_fun, std::ref(q));

    reader.join();
    writer.join();
*/
    return 0;
}
