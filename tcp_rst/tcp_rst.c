#include <stdio.h>      /* for printf() */
#include <sys/socket.h> /* for recv() */
#include <unistd.h>     /* for close() */

void DieWithError(char *errorMessage);  /* Error handling function */

void HandleTCPClient(int clntSocket)
{
    char recvChar;          /* Character to hold receive data */

    /* Receive a single byte from the client */
    if (recv(clntSocket, &recvChar, 1, 0) < 0)
        DieWithError("recv() failed");

    printf("Received a byte...now closing\n");

    /* shutdown(clntSocket, SHUT_WR); */
    close(clntSocket);

}
