#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>


#define PORT 50000
#define BUFFER 256

int main(int argc, char const *argv[])
{
    int waittime = 20;

    int consock;
    struct sockaddr_in serveraddr;

    int len;
    char sendmess[BUFFER], recvmess[BUFFER];
    consock = socket(AF_INET, SOCK_STREAM, 0);
    if(consock < 0)
    {
        fprintf(stderr, "client - socket() error !!\n");
        exit(1);
    }
    else
    {
        printf("created!!\n");
    }
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);
    bzero(&serveraddr.sin_zero, 8);


    if(connect(consock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        fprintf(stderr, "client-connect() error!\n");
        exit(1);
    }
    else
    {
        printf("connected!!\n");
    }
    
    do
    {
        printf("Enter mess: ");
        fgets(sendmess, BUFFER, stdin);
        len = strlen(sendmess);
        sendmess[len -1] = '\0';
        send(consock, sendmess, BUFFER, 0);

        wait(&waittime);

        recv(consock, recvmess, BUFFER, 0);
        printf("Receive mess: %s\n", recvmess);
    }while(strcmp(sendmess, "bye") != 0);


    close(consock);
    return 0;
}
