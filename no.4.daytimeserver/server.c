#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5000
#define QLEN 5
#define BUFFER 256

int main(int argc, char const *argv[])
{
    int sockfd, newfd;
    struct sockaddr_in serveraddr, clientaddr;
    time_t t;
    int addrlen;
    char sendmess[BUFFER];

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
        printf("Bind() successfull!\n");
    }

    listen(sockfd, QLEN);

    addrlen = sizeof(clientaddr);
    newfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
    if (newfd < 0)
    {
        fprintf(stderr, "Server-accept() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Accepted!\n");
    }
    time(&t);
    strcpy(sendmess, ctime(&t));
    if(send(newfd,sendmess, sizeof(sendmess), 0) < 0)
    {
        fprintf(stderr, "Server-send() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Send successful!\n");
    }
    

    close(newfd);
    close(sockfd);
    return 0;
}