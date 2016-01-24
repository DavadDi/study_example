#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "setproctitle.c"

int main(int argc, char *argv[])
{
    spt_init(argc,argv);
    setproctitle("this is a test");
    sleep(1000);
    return 0;

}
