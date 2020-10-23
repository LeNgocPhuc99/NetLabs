#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>

#define PORT 50000
#define LENQUEUE 5
#define BUFFER 256

int main(int argc, char const *argv[])
{
    int waittime = 20;
    int sockfd, newsock;
    char sendmess[BUFFER], recvmess[BUFFER];
    struct sockaddr_in serveraddr, cliaddr;

    int clilen, len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "server-socket() error!\n");
        exit(1);
    }
    else
    {
        printf("Create successful!\n");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);
    bzero(&serveraddr.sin_zero, 8);

    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        fprintf(stderr, "server-bind() error!\n");
        exit(1);
    }
    printf("Bind successfull!\n");
   
    listen(sockfd, LENQUEUE);

    clilen = sizeof(cliaddr);
    newsock = accept(sockfd, (struct sockaddr *)& cliaddr, &clilen);
    if(newsock < 0)
    {
        fprintf(stderr, "server-accept() error!! \n");
        exit(1);
    }
    else
    {
        printf("Accepted\n");
    }

    do
    {
        recv(newsock, recvmess, BUFFER, 0);
        printf("Receive mess: %s\n", recvmess);

        printf("Enter mess:");
        fgets(sendmess, BUFFER, stdin);
        len = strlen(sendmess);
        sendmess[len-1] = '\0';
        send(newsock, sendmess, BUFFER, 0);

        wait(&waittime);
    }while(strcmp(sendmess,"bye") != 0);

    close(newsock);
    close(sockfd);
    return 0;
}
