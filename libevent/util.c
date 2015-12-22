#include "util.h"

int my_printf(const char *fmt, ...)
{
    char buffer[1024];
    va_list argptr;
    int length = 0;

    va_start(argptr, fmt);
    length = vsnprintf(buffer,1024,fmt, argptr);
    va_end(argptr);

    printf("%s\n", buffer);
    return (length + 1);
}


