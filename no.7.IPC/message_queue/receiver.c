#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "queue.h"

int main(int argc, char const *argv[])
{
    key_t key = ftok(PathName, ProjectID);
    if(key < 0)
    {
        report_and_exit("key not gotten");
    }

    int qid = msgget(key, 0666 | IPC_CREAT);
    if(qid < 0)
    {
        report_and_exit("no access to queue");
    }

    int types[] = {3, 1, 2, 1, 3, 2};
    for(int i = 0; i < MsgCount; i++)
    {
        queueMessage msg;
        if(msgrcv(qid, &msg, sizeof(msg), types[i], MSG_NOERROR | IPC_NOWAIT) < 0)          /* receiver msg */
        {
            puts("msgrcv trouble");
        }
        printf("%s received as type %i\n", msg.payload, (int) msg.type);
    }


    /* remove the queue */
    if(msgctl(qid, IPC_RMID, NULL) < 0)
    {
        report_and_exit("trouble removing queue");
    }
    return 0;
}

