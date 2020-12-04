#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5000
#define QLEN 5

int main(int argc, char const *argv[])
{
    int sockfd, newfd;
    struct sockaddr_in serveraddr, clientaddr;
    int addrlen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "Server-socket() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Create successfull\n");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&serveraddr.sin_zero, 8);

    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        fprintf(stderr, "Server-bind() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("bind() successfull\n");
    }

    listen(sockfd, QLEN);

    addrlen = sizeof(clientaddr);
    newfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);

    if (newfd < 0)
    {
        fprintf(stderr, "server-accept() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Client adddress: %s\n", inet_ntoa(clientaddr.sin_addr));
    }

    close(sockfd);
    return 0;
}
