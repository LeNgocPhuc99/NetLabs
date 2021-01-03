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
    /* create message queue 1*/
    key_t key;
    int msgid;

    key = ftok(PathName, ProjectID2);
    msgid = msgget(key, 0666 | IPC_CREAT);

    do
    {   
        /* read data from messsage queue */
        bzero(&message1.mgs, BUFFER);
        msgrcv(msgid, &message1,sizeof(message1), 1, 0);
        printf("Data read from message queue 2: %s\n", message1.mgs);
        
    } while (strcmp(message1.mgs, "bye") != 0);

    msgctl(msgid, IPC_RMID, NULL); 
    return 0;
}