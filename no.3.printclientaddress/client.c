#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 5000

int main(int argc, char const *argv[])
{
    int connectfd;
    struct sockaddr_in  serveraddr;

    connectfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connectfd < 0)
    {
        fprintf(stderr, "Client-socket() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Created successfull\n");
    }
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(serveraddr.sin_zero, 8);

    if(connect(connectfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0 )
    {
        fprintf(stderr, "Server-connect() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Connected\n");
    }

    close(connectfd);
    return 0;
}


