#include <thread>
#include "readerwriterqueue.h"
using namespace moodycamel;

#include <unistd.h>

int main()
{
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

    return 0;
}
