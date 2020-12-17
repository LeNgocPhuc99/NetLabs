#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <sys/ipc.h>
#include <sys/msg.h>


#include "config.h"

struct msg message1;

int main(int argc, char const *argv[])
{
    int waittime = 20;
    int sockfd, newsock;
    char recvmess[BUFFER];
    struct sockaddr_in serveraddr, cliaddr;

    int clilen;
    long int len;

    /* create message queue 2 */
    key_t key;
    int msgid;
    key = ftok(PathName, ProjectID2);
    msgid = msgget(key, 0666 | IPC_CREAT);


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "server-socket() error!\n");
        exit(1);
    }
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORTSRV2);
    bzero(&serveraddr.sin_zero, 8);

    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        fprintf(stderr, "server-bind() error!\n");
        exit(1);
    }
    
    listen(sockfd, LISQUEUE);

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
        /* receive message from netdclient */
        recv(newsock, recvmess, BUFFER, 0);
        printf("Receive from netdclient: %s \n", recvmess);

        /* write in message queue 1 */
        message1.msg_type = 1;
        bzero(&message1.mgs, BUFFER);
        (void)strcpy(message1.mgs, recvmess);
        /* write message */
        msgsnd(msgid, &message1, sizeof(message1), 0);
        printf("Data write in message queue 2: %s \n", message1.mgs);

        wait(&waittime);

    } while (strcmp(recvmess, "bye") != 0);
    
    

    close(newsock);
    close(sockfd);
    return 0;
}