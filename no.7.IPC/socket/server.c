#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "sock.h"


void report(const char* msg, int terminate)
{
    perror(msg);
    if(terminate)
    {
        exit(-1);
    }
}


int main(int argc, char const *argv[])
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(fd < 0)
    {
        report("socket", 1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PortNumber);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(fd, (struct sockaddr*)&saddr, sizeof(saddr)) , 0)
    {
        report("bind", 1);
    }

    if(listen(fd, MaxConnects) < 0)
    {
        report("listen", 1);
    }

    fprintf(stderr, "Listening on port %i for client\n", PortNumber);
    while (1)
    {
        struct sockaddr_in caddr;
        int len = sizeof(caddr);

        int clientfd = accept(fd, (struct sockaddr*)& caddr, &len);
        if(clientfd < 0)
        {
            report("accept", 0);
            continue;
        }

        /* read data from client */
        for(int i = 0; i < ConversationLen; i++)
        {
            char buff[BuffSize + 1];
            bzero(buff, sizeof(buff));
            int count = read(clientfd, buff, sizeof(buff));
            if(count > 0)
            {
                puts(buff);
                write(clientfd, buff, sizeof(buff));
            }
        }
        close(clientfd);
    }
    
    return 0;
}
