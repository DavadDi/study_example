#include "itimer.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>

int g_run_flag = 1;

void sig_handler(int sig_no)
{
    if (sig_no == SIGINT)
    {
        g_run_flag = 0;
    }
}

void timer1(void *data, void *user)
{
    fprintf(stderr, "timer1 %d, %s, %s\n", (int)time(0), (char *)data, (char *)user);
}

int main(int argc, char *argv[])
{
    signal(SIGINT,sig_handler);

    itimer_mgr tm_mgr;

    itimer_mgr_init(&tm_mgr, time(0)*1000, 1);

    itimer_evt evt1;
    const char *pdata = "Hello";
    const char *user = "diwh";

    itimer_evt_init(&evt1, timer1,(void *)pdata, (void *)user);
    itimer_evt_start(&tm_mgr, &evt1, 5000, 0);

    while(g_run_flag)
    {
        fprintf(stderr, "Call itimer_mgr_run\n");
        itimer_mgr_run(&tm_mgr, time(0)*1000);
        sleep(1);
    }


    itimer_evt_stop(&tm_mgr, &evt1);
    itimer_evt_destroy(&evt1);

    itimer_mgr_destroy(&tm_mgr);

    return 0;
}
