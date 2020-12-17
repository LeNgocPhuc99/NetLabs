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

struct msg message;

int main(int argc, char const *argv[])
{
    int consock;
    struct sockaddr_in serveraddr;

    int len;
    int waittime = 20;
    /* create message queue 1*/
    key_t key;
    int msgid;

    key = ftok(PathName, ProjectID1);
    msgid = msgget(key, 0666 | IPC_CREAT);

    /* connect to netdserver */
    consock = socket(AF_INET, SOCK_STREAM, 0);
    if(consock < 0)
    {
        fprintf(stderr,  "netdclient-socket() error !\n");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORTSRV2);
    bzero(&serveraddr.sin_zero, 8);

    if(connect(consock, (struct sockaddr*)& serveraddr, sizeof(serveraddr)) < 0)
    {
        fprintf(stderr, "netdclient-connect() error!\n");
        exit(1);
    }
    else
    {
        printf("Connected\n");
    }

    do
    {   
        /* read data from messsage queue */
        bzero(&message.mgs, BUFFER);
        msgrcv(msgid, &message,sizeof(message), 1, 0);
        printf("Data read from message queue 1: %s\n", message.mgs);

        /* send to netdserver */
        send(consock, message.mgs, BUFFER, 0);
        
    } while (strcmp(message.mgs, "bye") != 0);
    
    msgctl(msgid, IPC_RMID, NULL); 
   
    close(consock);
    return 0;
}
