#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#define PORT 4000
#define BUFFER 256

int main(int argc, char const *argv[])
{
    int sfd;
    struct sockaddr_in saddr, caddr;
    int len;
    char rbuff[BUFFER];


    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sfd < 0)
    {
        fprintf(stderr, "server-socket() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("created!\n");
    }
    

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(PORT);
    bzero(&saddr.sin_zero, 8);


    if (bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        fprintf(stderr, "server-bind() error: %d\n",errno);
        exit(1);
    }
    else
    {
        printf("bind successful!\n");
    }
    
    len = sizeof(caddr);
    if(recvfrom(sfd, &rbuff, sizeof(BUFFER), 0, (struct sockaddr *)&caddr, &len) < 0)
    {
        fprintf(stderr,"server - recvfrom error: %d\n", errno);
        exit(1);
    }
    else
    {
        rbuff[strlen(rbuff) - 1] ='\0';
        printf("server received: %s\n", rbuff);
    }
    close(sfd);
    return 0;
}
