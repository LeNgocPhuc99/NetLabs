#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 5000
#define QLEN 5

int main(int argc, char const *argv[])
{
    int sockfd, newsock;
    struct sockaddr_in serveraddr, clientaddr;
    int addrlen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        fprintf(stderr, "Sever-socket() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Created!\n");
    }

    serveraddr.sin_family  = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&serveraddr.sin_zero, 8);

    if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        fprintf(stderr, "Server-bind() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Bind successful!\n");
    }

    listen(sockfd, QLEN);

    addrlen = sizeof(clientaddr);
    newsock = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
    if(newsock < 0)
    {
        fprintf(stderr, "Server-accept() error: %d\n", errno);
        exit(0);
    }
    else
    {
        printf("Accepted!\n");
    }
    

    
    return 0;
}
