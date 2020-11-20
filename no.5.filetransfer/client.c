#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER 256
#define PORT 5000


int main(int argc, char const *argv[])
{
    int consock;
    struct sockaddr_in serveraddr;
    char filePath[BUFFER];
    
    consock = socket(AF_INET, SOCK_STREAM, 0);
    if(consock < 0)
    {
        fprintf(stderr, "Client-socket() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Created\n");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&serveraddr.sin_zero, 8);

    if(connect(consock, (struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
    {
        fprintf(stderr, "Client-connect error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Connected!\n");
    }

    
    return 0;
}
