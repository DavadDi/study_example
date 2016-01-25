#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

int main()
{
    struct rlimit limits;

#if 0
  struct rlimit
  {
        rlim_t rlim_cur;  /* Soft limit */
        rlim_t rlim_max;  /* Hard limit (ceiling for rlim_cur) */
  };
#endif

    /* RLIMIT_NOFILE one process can open max files */
    if ( getrlimit( RLIMIT_NOFILE, &limits ) == 0  )
    {
        if (limits.rlim_cur == RLIM_INFINITY)
            printf ("RLIMIT_NOFILE Soft Limit is infinite\n");
        else
            printf ("RLIMIT_NOFILE soft limit: %lld\n", (long long int)limits.rlim_cur);

        if ( limits.rlim_max == RLIM_INFINITY  )
            printf ("RLIMIT_NOFILE Hard Limit is infinite\n");
        else
            printf ("RLIMIT_NOFILE hard limit: %lld\n", (long long int)limits.rlim_max);

    }

    printf ("Now, use setrlimit() function to change this limit.\n");

    limits.rlim_max = 8192;
    (void) setrlimit(RLIMIT_NOFILE, &limits);
    limits.rlim_cur = 8192;
    (void) setrlimit(RLIMIT_NOFILE, &limits);

    if ( getrlimit( RLIMIT_NOFILE, &limits ) == 0  )
    {
        if (limits.rlim_cur == RLIM_INFINITY)
            printf ("RLIMIT_NOFILE Soft Limit is infinite\n");
        else
            printf ("RLIMIT_NOFILE soft limit: %lld\n", (long long int)limits.rlim_cur);

        if ( limits.rlim_max == RLIM_INFINITY  )
            printf ("RLIMIT_NOFILE Hard Limit is infinite\n");
        else
            printf ("RLIMIT_NOFILE hard limit: %lld\n", (long long int)limits.rlim_max);

    }

    return 0;
}
