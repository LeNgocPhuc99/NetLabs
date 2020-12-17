#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <string.h>

#define BUFFER      256 
#define MAX         10
#define ProjectID       111
#define PathName        "message.h" 

#include "message.h"


int main() 
{ 

    struct message Message; 
    key_t key; 
    int msgid; 
  
    //generate unique key 
    key = ftok(PathName, ProjectID); 
  
    //creates a message queue 
    msgid = msgget(key, 0666 | IPC_CREAT); 
    Message.msg_type = 1; 
    bzero(&Message.msg, BUFFER);

  
    (void)strcpy(Message.msg, "Hello!");
    
  
    //send message 
    msgsnd(msgid, &Message, sizeof(Message), 0); 

    printf("Data send is : %s \n", Message.msg); 
  
    return 0; 
} 
