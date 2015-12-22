#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>

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
    my_struct_t *myStruct = reinterpret_cast<my_struct_t *>(arg);

    // Cleanup our properties
    event_free(myStruct->cleanup);
    evhttp_connection_free(myStruct->conn);
    evhttp_uri_free(myStruct->uri);

    // Free our custom structure
    free(myStruct);
}

void http_printf_header(struct evhttp_request *req)
{
    int response_code = evhttp_request_get_response_code(req);
    const char *request_url = evhttp_request_get_uri(req);
    printf("request from url[%s], response code [%d]\n", request_url,response_code);
    struct evkeyvalq* headers = evhttp_request_get_input_headers(req);
    for (struct evkeyval* header = headers->tqh_first; header;
        header = header->next.tqe_next) {
        printf("%s:%s\n", header->key, header->value);
    }
	printf("\n\n");
}

void http_request_done(struct evhttp_request *req, void *arg){

    // Get our custom struct
    my_struct_t *myStruct = reinterpret_cast<my_struct_t *>(arg);
    
    http_printf_header(req);
    // Setup our timeout information (we delay 5 seconds)
    struct timeval Timeout;
    Timeout.tv_sec = 0;
    Timeout.tv_usec = 0;

    // Add this structure to our cleanup base to be cleaned up synchronously
    // TODO: Probably not the best way to cleanup and event, but it'l work for the purposes of illustration.
    // This way would ensure no race conditions exist, but it's probably not the most efficient depending on how many requests, etc we're dealing with.
    myStruct->cleanup = evtimer_new(Base_Primary, connection_free, (void *)myStruct);
    evtimer_add(myStruct->cleanup, &Timeout);

    //
    printf("http_request_done, we put our custom strucutre into a cleanup event to be freed!\n");
}

int http_req(const char *uri, int i) {

    // Allocate our custom struture
    my_struct_t *myStruct = reinterpret_cast<my_struct_t*>(malloc(sizeof(my_struct_t)));
    // Create our EVHTP connection and request
    myStruct->uri = evhttp_uri_parse(uri);
	const char *host = evhttp_uri_get_host(myStruct->uri);
	printf("GET host [%s]\n", host);
    myStruct->conn = evhttp_connection_base_new(Base_Primary, NULL, evhttp_uri_get_host(myStruct->uri), evhttp_uri_get_port(myStruct->uri) != -1 ? evhttp_uri_get_port(myStruct->uri) : 80);
    myStruct->req = evhttp_request_new(http_request_done, myStruct);
	struct evkeyvalq * output_header = myStruct->req->output_headers;
    evhttp_add_header(output_header, "Host", host);
    evhttp_add_header(output_header, "Connection", "keep-alive");
    evhttp_add_header(output_header, "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*; q=0.8");
    evhttp_add_header(output_header, "Accept-Encoding", "gzip, deflate, sdch");
    evhttp_add_header(output_header, "Accept-Language", "zh-CN, zh; q=0.8");
    evhttp_add_header(output_header, "User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36");
    const char* path = evhttp_uri_get_path(myStruct->uri);
	printf("path [%s]\n", path);
	evhttp_make_request(myStruct->conn, myStruct->req, EVHTTP_REQ_GET, path);
    return 1;
}


// Define our primary function
int main(int argc, char *argv[]) {

    // Initialize our bases
    Base_Primary = event_base_new();

    http_req("https://www.google.com/", 1);
    http_req("https://www.yahoo.com/", 2);
    http_req("http://10.1.11.34:8090/msg_server", 3);

    //
    event_base_loop(Base_Primary, 0);

    // Free our primary base
    event_base_free(Base_Primary);
    return 1;
}
