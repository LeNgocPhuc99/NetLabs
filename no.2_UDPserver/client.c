#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4000
#define BUFFER 256

int main(int argc, char const *argv[])
{
    int cfd;
    struct sockaddr_in saddr;
    char ipaddr[40];
    char data[BUFFER];

    cfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(cfd < 0)
    {
        fprintf(stderr, "client-socket() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("create successfull!\n");
    }

    printf("Enter IP: ");
    scanf("%s",ipaddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    inet_aton(ipaddr, &saddr.sin_addr);
    bzero(&saddr.sin_zero, 8);

    printf("Enter data: ");
    scanf("%s",data);
    //data[strlen(data)] = '\0';
    
    if (sendto(cfd,data, sizeof(data), 0, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        fprintf(stderr, "client-sendto() error: %d\n", errno);
        exit(1);
    }
    else
    {
        printf("Server transmitted: %s\n", data);
    }
    close(cfd);
    return 0;
}
