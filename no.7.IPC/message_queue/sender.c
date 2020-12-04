#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"




int main(int argc, char const *argv[])
{
    
    key_t key = ftok(PathName, ProjectID);              /* generate key */
    if(key < 0)
    {
        report_and_exit("couldn't get key");
    }

    int qid = msgget(key, 0666 | IPC_CREAT);            /* use key to get queue */            
    if (qid < 0)
    {
        report_and_exit("couldn't get queue id");
    }

    char* payloads[] = {"msg1", "msg2", "msg3", "msg4", "mgs5", "mgs6"};
    int type[] = {1, 1, 2, 2, 3, 3};

    for(int i = 0; i < MsgCount; i++)
    {
        /* make message */
        queueMessage msg;
        msg.type = type[i];
        strcpy(msg.payload, payloads[i]);

        /* send message */
        msgsnd(qid, &msg, sizeof(msg), IPC_NOWAIT);
        printf("%s send as type %i\n", msg.payload, (int) msg.type);
    }
    
    return 0;
}
