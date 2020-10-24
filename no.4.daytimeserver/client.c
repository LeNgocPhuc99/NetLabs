#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER 256

int main(int argc, char const *argv[])
{
    int consock;
    struct sockaddr_in serveraddr;
    int addrlen;
    char recvbuff[BUFFER];

    consock = socket(AF_INET, SOCK_STREAM, 0);
    if(consock < 0)
    {
        fprintf(stderr, "Client-socket() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Created\n");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&serveraddr.sin_zero, 8);

    if(connect(consock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        fprintf(stderr, "Client-connect() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Connected\n");
    }

    addrlen = sizeof(serveraddr);
    if(recv(consock, &recvbuff, sizeof(recvbuff), 0) < 0)
    {
        fprintf(stderr, "Client-recv() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Day-Time: %s\n",recvbuff);
    }
    return 0;
}

