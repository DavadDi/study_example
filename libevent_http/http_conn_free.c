#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <evhttp.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/bufferevent.h>

#include <stdarg.h>

#include <signal.h>

typedef struct my_struct_s my_struct_t;

struct my_struct_s {
    struct evhttp_connection *conn;
    struct evhttp_request *req;
    struct evhttp_uri *uri;
    struct event *cleanup;
};

struct event_base *Base_Primary;

void connection_free(int sock, short which, void *arg) {
    printf("freeing connection!!! The socket's FD would have been closed when the HTTP request ended and the ->req object would have been free'd\n");

    // Get our structure object
    my_struct_t *myStruct = arg;

    // Cleanup our properties
    event_free(myStruct->cleanup);
    evhttp_connection_free(myStruct->conn);
    evhttp_uri_free(myStruct->uri);

    // Free our custom structure
    free(myStruct);
}

void http_request_done(struct evhttp_request *req, void *arg){

    // Get our custom struct
    my_struct_t *myStruct = arg;

    // Setup our timeout information (we delay 5 seconds)
    struct timeval Timeout;
    Timeout.tv_sec = 5;
    Timeout.tv_usec = 0;

    // Add this structure to our cleanup base to be cleaned up synchronously
    // TODO: Probably not the best way to cleanup and event, but it'l work for the purposes of illustration.
    // This way would ensure no race conditions exist, but it's probably not the most efficient depending on how many requests, etc we're dealing with.
    myStruct->cleanup = evtimer_new(Base_Primary, connection_free, (void *)myStruct);
    evtimer_add(myStruct->cleanup, &Timeout);

    //
    printf("http_request_done, we put our custom strucutre into a cleanup event to be freed!\n");
}

int http_req(char *uri, int i) {

    // Allocate our custom struture
    my_struct_t *myStruct = malloc(sizeof(my_struct_t));

    // Create our EVHTP connection and request
    myStruct->uri = evhttp_uri_parse(uri);
    myStruct->conn = evhttp_connection_base_new(Base_Primary, NULL,
            evhttp_uri_get_host(myStruct->uri),
            evhttp_uri_get_port(myStruct->uri) != -1 ? evhttp_uri_get_port(myStruct->uri) : 80);

    myStruct->req = evhttp_request_new(http_request_done, myStruct);
    evhttp_add_header(evhttp_request_get_output_headers(myStruct->req), "Host", "localhost");
    //evhttp_add_header(evhttp_request_get_output_headers(myStruct->req), "Connection", "close");
    evhttp_make_request(myStruct->conn, myStruct->req, EVHTTP_REQ_GET, uri);

    return 1;
}


// Define our primary function
int main(int argc, char *argv[]) {

    int loopNum = 1;

    if (argc == 2)
    {
        loopNum = atoi(argv[1]);
    }
    // Initialize our bases
    Base_Primary = event_base_new();

    fprintf(stderr, "Start num %d\n", loopNum);
    for (int i = 0; i < loopNum; i++)
    {
        http_req("http://10.1.11.14:8500/v1/catalog/service/web", 2);
    }

    event_base_dispatch(Base_Primary);

    // Free our primary base
    event_base_free(Base_Primary);
    return 1;
}
