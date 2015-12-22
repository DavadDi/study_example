#include "util.h"

/* For sockaddr_in */
#include <netinet/in.h>
#include <arpa/inet.h>

/* For socket functions */
#include <sys/socket.h>

/* For fcntl */
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>


int start_client(const char *host, uint16_t port, struct event_base *base);
int start_server(uint16_t port, struct event_base *base);

int main(int argc, char *argv[])
{
    struct event_base *base;
    const char        *version = event_get_version();
    uint16_t port = 5060;

    if (argc > 1)
    {
        port = atoi(argv[1]);
    }

    // No buffering: No buffer is used. Each I/O operation is written as soon as possible.
    // In this case, the buffer and size parameters are ignored.
    setvbuf(stdout, NULL, _IONBF, 0);

    LOG_INFO("libevent version: %s", version);
    LOG_INFO("The Server will be listenning at port [%d]", port);

    base = event_base_new();
    if (!base)
    {
        LOG_ERROR("event_base_new() failed.");
        return -1;
    }

    start_server(port, base);
    start_client("127.0.0.1", port, base);

    event_base_dispatch(base);
    // event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);
    // event_base_loopbreak(base);  on sig handler
    // void event_base_dump_events(struct event_base *base, FILE *f);
    return 0;

    // TODO:
    // signal, timer
    //

}

void read_cb(struct bufferevent *bev, void *ctx)
{
    struct evbuffer *input, *output;
    char *line;
    size_t n;
    int i;

    input = bufferevent_get_input(bev);
    output = bufferevent_get_output(bev);

    while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF)))
    {
        evbuffer_add(output, line, n);
        evbuffer_add(output, "\n", 1);
        free(line);
    }

    if (evbuffer_get_length(input) >= MAX_LINE)
    {
        /* Too long; just process what there is and go on so that the buffer
         * doesn't grow infinitely long. */
        char buf[1024];
        while (evbuffer_get_length(input))
        {
            int n = evbuffer_remove(input, buf, sizeof(buf));
            evbuffer_add(output, buf, n);
        }

        evbuffer_add(output, "\n", 1);
    }
}


void add_read_timeout(struct bufferevent *bev, int second)
{
    // add timeout deal
    struct timeval tv;
    tv.tv_sec = second; // 10s
    tv.tv_usec = 0;

    bufferevent_set_timeouts(bev, &tv, NULL);
}

void event_cb(struct bufferevent *bev, short event, void *arg)
{
    evutil_socket_t sock_fd = (evutil_socket_t)arg;

    if (event & BEV_EVENT_CONNECTED)
    {
        LOG_INFO("Connect to Server succeed!");
        // client connected to server
    }
    else if (event & BEV_EVENT_EOF)
    {
        /* connection has been closed, do any clean up here */
        LOG_INFO("event_cb() Peer close the socket fd [%d]", sock_fd);

        /* will call close socket */
        bufferevent_free(bev);
    }
    else if (event & BEV_EVENT_ERROR)
    {
        /* check errno to see what error occurred */
        LOG_ERROR("event_cb() someting is wrong on sock_fd [%d]. error %s",
                sock_fd, evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));

        bufferevent_free(bev);
    }
    else if (event & BEV_EVENT_TIMEOUT)
    {
        /* must be a timeout event handle, handle it */
        LOG_INFO("event_cb() timeout on sock_fd [%d]", sock_fd);

        // don't work here add.
        // add_read_timeout(bev, 10);
        // int event_base_gettimeofday_cached(struct event_base *base, struct timeval *tv_out)
    }
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int cli_fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (cli_fd < 0)
    {
        LOG_ERROR("Aceept failed! errno %d, %s", errno, strerror(errno));
        return;
    }
    else if (cli_fd > FD_SETSIZE)
    {
        LOG_INFO("Fd is exceed max fd-set number!");
        close(cli_fd);
        return;
    }

    struct bufferevent *bev;
    evutil_make_socket_nonblocking(cli_fd);

    char str0[655350];
    char *str1 = "world\n";
    struct iovec iov[2];
    ssize_t nwritten;

    iov[0].iov_base = str0;
    iov[0].iov_len = 65535;
    iov[1].iov_base = str1;
    iov[1].iov_len = strlen(str1);

    for (int i = 0; i < 100; i++)
    {
        nwritten = writev(cli_fd, iov, 2);
        LOG_INFO("Writev res %d, errno %d, %s", nwritten, errno, strerror(errno));
        if (nwritten < 0)
        {
            break;
        }
    }

    bev = bufferevent_socket_new(base, cli_fd, BEV_OPT_CLOSE_ON_FREE);

    /* read_cb, write_cb, event_cb, cbarg */
    bufferevent_setcb(bev, read_cb, NULL, event_cb, (void *)cli_fd);

    bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
    bufferevent_enable(bev, EV_READ|EV_WRITE);

    add_read_timeout(bev, 10);
}

int start_server(uint16_t port, struct event_base *base)
{
    evutil_socket_t listener;
    struct sockaddr_in sin;
    struct event *listener_event;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(listener);

#ifndef WIN32
    {
        int one = 1;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    }
#endif

    if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        LOG_ERROR("Bind on port [%d] failed! errno %d, %s", port, errno, strerror(errno));
        return -1;
    }

    if (listen(listener, 16) < 0)
    {
        LOG_INFO("Listen faield. errno %d, %s", errno, strerror(errno));
        return -1;
    }

    listener_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);
    if (!listener_event)
    {
        LOG_ERROR("Event new for listen failed.");
        return -1;
    }

    event_add(listener_event, NULL);

    return 0;
}

int start_client(const char *host, uint16_t port, struct event_base *base)
{
    struct bufferevent *bev;
    struct sockaddr_in sin;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(host); /* 127.0.0.1 */
    sin.sin_port = htons(port); /* Port 8080 */

    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);
    if (bufferevent_socket_connect(bev,
                (struct sockaddr *)&sin, sizeof(sin)) < 0) { /* Error starting connection */ bufferevent_free(bev);
        return -1;
    }

    return 0;
}
