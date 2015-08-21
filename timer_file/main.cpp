#include "timer_file.hpp"

int main()
{
    CTimerFileInfo tf_info;

    tf_info.file_prefix("mysql");
    tf_info.file_suffix("log");
    tf_info.file_dir("./data/");

    tf_info.file_max_size(1024*1024);   // 1m
    tf_info.file_max_line(10000);      // 10w

    tf_info.file_max_timeout(60*60);   // one hour
    tf_info.file_start_seq(0);

    CTimerFile timer_file(tf_info);

    char buf[1024];
    for (int i = 1; i < 1000000; i++)
    {
        snprintf(buf, 1024, "hellllllllllllllllllllll_%d\n", i);
        
        if (i%10000 == 0)
        {
            LOG_DEBUG("Sleep 1s");
            sleep(1);
        }

        if ( timer_file.write_data(buf, strlen(buf)) == -1)
        {
            fprintf(stderr, "write faied, break\n");
            break;
        }
    }

    return 0;
}
