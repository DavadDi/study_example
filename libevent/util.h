#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>


int my_printf(const char *fmt, ...);
#define LOG_DEBUG my_printf
#define LOG_WARN  my_printf
#define LOG_INFO  my_printf
#define LOG_ERROR my_printf
#define LOG_TRACE_METHORD my_printf

#define MAX_LINE  4096

