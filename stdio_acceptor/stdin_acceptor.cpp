#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4711


int main(void) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    bind(sockfd, (struct sockaddr*) &addr, sizeof (addr));
    listen(sockfd, 10);

    int epollfd = epoll_create1(0);
    struct epoll_event event;
    
    // add stdin
    event.events = EPOLLIN|EPOLLPRI|EPOLLERR;
    event.data.fd = STDIN_FILENO;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &event) != 0) {
        perror("epoll_ctr add stdin failed.");
        return 1;
    }
    
    // add socket
    event.events = EPOLLIN|EPOLLPRI|EPOLLERR;
    event.data.fd = sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) != 0) {
        perror("epoll_ctr add sockfd failed.");
        return 1;
    }

    fprintf(stderr, ">");
    char *line = NULL;
    size_t linelen = 0;
    for (;;) {
        int fds = epoll_wait(epollfd, &event, 1, -1);
        if (fds < 0) {
            perror("epoll_wait failed.");
            return 2;
        }
        if (fds == 0) {
            continue;
        }

        if (event.data.fd == STDIN_FILENO) {
            // read input line
            int read = getline(&line, &linelen, stdin);
            if (read < 0) {
                perror("could not getline");
                return 3;
            }
            printf("Read: %.*s", read, line);
            fprintf(stderr, ">");

        } else if (event.data.fd == sockfd) {
            // accept client
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof (client_addr);
            int clientfd = accept(sockfd, (struct sockaddr*) &client_addr, &addrlen);
            if (clientfd == -1) {
                perror("could not accept");
                return 4;
            }

            const char *str = "This is a test. Bye\n";
            send(clientfd, str, strlen(str), 0);
            close(clientfd);
        } else {
            // cannot happen™
            fprintf(stderr, "Bad fd: %d\n", event.data.fd);
            return 5;
        }
    }

    close(epollfd);
    close(sockfd);
    return 0;

}
