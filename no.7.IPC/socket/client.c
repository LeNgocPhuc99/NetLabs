#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include "sock.h"

const char* books[] = {"War and Peace",
                        "Pride and Prejudice",
                        "The Sound and the Fury"};

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
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    { 
        report("socket", 1);
    }

    /* get address by host */
    struct hostent* host = gethostbyname(Host);
    if(!host)
    {
        report("gethostbyname", 1);
    }

    if(host->h_addrtype != AF_INET)
    {
        report("bad address", 1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = ((struct in_addr*) host->h_addr_list[0])->s_addr;
    saddr.sin_port = htons(PortNumber);

    if(connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
    {
        report("accept",1);
    }

    puts("Connect to server");
    
    for(int i = 0; i < ConversationLen; i++) 
    {
        if(write(sockfd, books[i], strlen(books[i])) > 0)
        {
            /* get response from server */
            char buff[BuffSize + 1];
            bzero(buff, sizeof(buff));
            if(read(sockfd, buff, sizeof(buff)) > 0)
            {
                puts(buff);
            }
        }
    }


    puts("Client done");
    close(sockfd);

    return 0;
}


