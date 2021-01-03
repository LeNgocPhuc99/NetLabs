#include <stdio.h>
#include <unistd.h>             // close(), read()
#include <sys/epoll.h>          // epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>

#define MAX_EVENTS 5
#define READ_SIZE 10


int main(int argc, char const *argv[])
{
    struct epoll_event event, events[MAX_EVENTS];

    // test
    int running = 1, event_count, i;
    size_t bytes_read;
    char read_buffer[READ_SIZE + 1];

    // create epoll instance
    int epoll_fd = epoll_create1(0);

    if(epoll_fd == -1)
    {
        fprintf(stderr, "Failed to create epoll\n");
        return 1;
    }

    // add fd for epoll to watch
    // watch only input events (EPOLLIN) on fd 0 (Standard Input)
    event.events = EPOLLIN;
    event.data.fd = 0;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event))
    {
        fprintf(stderr, "Failed to add file descriptor to epoll\n");
        close(epoll_fd);
        return 1;
    }

    while (running)
    {
        printf("\nPolling for input..\n");
        // wait for events to occur form epoll instance (epoll_fd), result stored in the events array. MAX_EVENTS events, timeout 30s
        event_count = epoll_wait(epoll_fd,events, MAX_EVENTS, 30000);
        printf("%d ready events\n", event_count);

        for ( i = 0; i < event_count; i++)
        {
            printf("Reading file descriptor '%d' --", events[i].data.fd);
            bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
            printf("%zd bytes read.\n", bytes_read);
            read_buffer[bytes_read] = '\0';
            printf("Read '%s' \n", read_buffer);

            if(!strncmp(read_buffer, "stop\n", 5))
            {
                running = 0;
            }
        }
        
    }
    
    if(close(epoll_fd))
    {
        fprintf(stderr, "Failed to close epoll\n");
        return 1;
    }
    return 0;
}
