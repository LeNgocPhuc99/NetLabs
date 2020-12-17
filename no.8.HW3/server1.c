#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <netinet/in.h>

#include "config.h"

struct msg message;

int main(int argc, char const *argv[])
{
    int sockfd, newsocket;
    char sendmess[BUFFER], recvmess[BUFFER];

    struct sockaddr_in serveraddr, clientaddr;
    int clientlen;
    long int len;
    int waittime = 20;

    /* create message queue 1 */
    key_t key;
    int msgid;
    /* generate uniqueue key */
    key = ftok(PathName, ProjectID1);
    /* create a message queue */
    msgid = msgget(key, 0666 | IPC_CREAT);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        fprintf(stderr, "server-socket() error!!\n");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORTSRV1);
    bzero(&serveraddr.sin_zero, 8);

    if(bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        fprintf(stderr,"server-bind() error!\n");
        exit(1);
    }

    listen(sockfd, LISQUEUE);

    clientlen = sizeof(clientaddr);
    newsocket = accept(sockfd, (struct sockaddr*)&clientaddr, &clientlen);
    if(newsocket < 0)
    {
        fprintf(stderr, "server-accept() error! \n");
        exit(1);
    }
    else 
    {
        printf("Accepted!\n");
    }
    
    do
    {
        /* received data from client */
        recv(newsocket, recvmess, BUFFER, 0);
        printf("Receive from client: %s\n", recvmess);

        /* send to client */
        for (int i = 0; i < strlen(recvmess); i++)
        {
            sendmess[i] = toupper(recvmess[i]);
        }

        len = strlen(recvmess);
        sendmess[len] = '\0';
        
        printf("Send message to client: %s\n", sendmess);
        send(newsocket, sendmess, BUFFER, 0);

        /* write in message queue 1 */
        message.msg_type = 1;
        bzero(&message.mgs, BUFFER);
        (void)strcpy(message.mgs, recvmess);
        /* write message */
        msgsnd(msgid, &message, sizeof(message), 0);
        printf("Data write in message queue 1: %s \n", message.mgs);

        wait(&waittime);
        
    } while (strcmp(recvmess, "bye") != 0);
    
    close(newsocket);
    close(sockfd);
    return 0;
}
