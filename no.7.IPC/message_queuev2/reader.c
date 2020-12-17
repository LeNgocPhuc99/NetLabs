
// C Program for Message Queue (Reader Process) 
#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 

#include "message.h"


#define BUFFER      256 
#define MAX         10
#define ProjectID       111
#define PathName        "message.h"


  
int main() 
{
    struct message message;
    key_t key; 
    int msgid; 
  
    //generate unique key 
    key = ftok(PathName, ProjectID); 
  
    // creates a message queue  
    msgid = msgget(key, 0666 | IPC_CREAT); 
  
    // receive message 
    msgrcv(msgid, &message, sizeof(message), 1, 0); 
  
 
    printf("Data Received is : %s \n", message.msg); 
  
    // to destroy the message queue 
    msgctl(msgid, IPC_RMID, NULL); 
  
    return 0; 
} 

