#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>


int main()
{
    int i;
    const char **methods = event_get_supported_methods();
    printf("Starting Libevent %s. Available methods are:\n", event_get_version());


    for (i=0; methods[i] != NULL; ++i)
    {
        printf("    %s\n", methods[i]);
    }

    return 0;
}
