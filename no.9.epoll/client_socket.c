#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>


#include "network.h"
#include "epoll_interface.h"
#include "backend_socket.h"
#include "client_socket.h"


#define BUFFER_SIZE 4096

//link list
struct data_buffer_entry 
{
    int is_close_message;
    char* data;
    int current_offset;
    int len;
    struct data_buffer_entry* next;
};



void really_close_client_socket(struct epoll_event_handler* self)
{

    // check buffer befor close client socket ==> avoid memory leak
    struct client_socket_event_data* closure = (struct client_socket_event_data* ) self->closure;
    struct data_buffer_entry* next;
    while (closure->write_buffer != NULL)
    {
        next = closure->write_buffer->next;
        if(!closure->write_buffer->is_close_message)
        {
            free(closure->write_buffer->data);
        }

        free(closure->write_buffer);
        closure->write_buffer = next;
    }
    
    close(self->fd);
    free(self->closure);
    free(self);
}



bool make_request(char* buffer, char* backend_addr)
{

    char command[BUFFER_SIZE];                  // GET
    char url[BUFFER_SIZE];                      // http://example.com/
    char http[BUFFER_SIZE];                     // HTTP/1.1
    char host[BUFFER_SIZE];                     // example.com
    char temp[BUFFER_SIZE];                     // http://example.com/
    char *entry;

    bool flag = false;

    sscanf(buffer, "%s %s %s", command, url, http);
    if(strcmp(command,"GET") == 0)
    {
        strcpy(temp, url);

        // get hostname
        entry = strtok(buffer, "//");
        entry = strtok(NULL, "/");
        strcpy(host, entry);

        if(strcmp(host, "172.16.45.130") != 0)
        {
            return flag;
        }
        else
        {
            bzero(host, BUFFER_SIZE);
            strcpy(host, backend_addr);
        }
        entry = strtok(temp, "//");
        entry = strtok(NULL, "/");
        entry = strtok(NULL, "\0");

        // make request
        bzero(buffer, BUFFER_SIZE);
        if(entry == NULL)
        {   
            sprintf(buffer, "%s / %s\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", command, http, host);
        }
        else
        {
            sprintf(buffer, "%s /%s %s\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", command, entry, http, host);
        }
        printf("Request:\n %s", buffer);
        flag = true;
        return flag;
    }
    else
    {
        return flag;
    } 
}


// fix bugs write()

void add_write_buffer_entry(struct client_socket_event_data* closure, struct data_buffer_entry* new_entry) 
{
    struct data_buffer_entry* last_buffer_entry;
    if (closure->write_buffer == NULL) 
    {
        closure->write_buffer = new_entry;
    } 
    else 
    {
        last_buffer_entry = closure->write_buffer;
        while (last_buffer_entry->next != NULL)
        {
            last_buffer_entry->next;
        }
        
        //for (last_buffer_entry=closure->write_buffer; last_buffer_entry->next != NULL; last_buffer_entry=last_buffer_entry->next);
        last_buffer_entry->next = new_entry;
    }
}


void write_to_client(struct epoll_event_handler* self, char* data, int len)
{
    struct client_socket_event_data* closure = (struct client_socket_event_data* ) self->closure;

    int written = 0;
    if (closure->write_buffer == NULL) 
    {
        written = write(self->fd, data, len);
        if (written == len) 
        {
            return;
        }
    }
    if (written == -1) 
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK) 
        {
            perror("Error writing to client");
            exit(-1);
        }
        written = 0;
    }

    int unwritten = len - written;
    struct data_buffer_entry* new_entry = malloc(sizeof(struct data_buffer_entry));
    new_entry->is_close_message = 0;
    new_entry->data = malloc(unwritten);
    memcpy(new_entry->data, data + written, unwritten);
    new_entry->current_offset = 0;
    new_entry->len = unwritten;
    new_entry->next = NULL;

    add_write_buffer_entry(closure, new_entry);
}

// check buffer before close
void close_client_socket(struct epoll_event_handler* self)
{
    struct client_socket_event_data* closure = (struct client_socket_event_data* ) self->closure;
    // no data in buffer
    if (closure->write_buffer == NULL) 
    {
        really_close_client_socket(self);
    } 
    else 
    {
        // add new_entry with flag is_close_message = 1 and no data
        struct data_buffer_entry* new_entry = malloc(sizeof(struct data_buffer_entry));
        new_entry->is_close_message = 1;
        new_entry->next = NULL;

        add_write_buffer_entry(closure, new_entry);
    }
}

void handle_client_socket_event(struct epoll_event_handler* self, uint32_t events)
{
    struct client_socket_event_data* closure = (struct client_socket_event_data* ) self->closure;

    // send data in the buffer to the client if event is EPOLLOUT and buffer has data
    if ((events & EPOLLOUT) && (closure->write_buffer != NULL)) 
    {
        int written;
        int to_write;
        struct data_buffer_entry* temp;
         // send until buffer is empty
        while (closure->write_buffer != NULL) 
        {
            // check close message
            if (closure->write_buffer->is_close_message) 
            {
                really_close_client_socket(self);
                return;
            }

            to_write = closure->write_buffer->len - closure->write_buffer->current_offset;
            // write data to client from current_offset
            written = write(self->fd, closure->write_buffer->data + closure->write_buffer->current_offset, to_write);
            // error
            if (written != to_write) 
            {
                if (written == -1) 
                {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) 
                    {
                        perror("Error writing to client");
                        exit(-1);
                    }
                    written = 0;
                }
                closure->write_buffer->current_offset += written;
                break;
            } 
            else 
            {
                // all data in a buffer write success
                // delete first node in link list
                temp = closure->write_buffer;
                closure->write_buffer = closure->write_buffer->next;
                free(temp->data);
                free(temp);
            }
        }
    }

    char read_buffer[BUFFER_SIZE];
    int bytes_read;

    // read data from client and send to backend server if event is EPOLLIN
    if (events & EPOLLIN) 
    {
        while ((bytes_read = read(self->fd, read_buffer, BUFFER_SIZE)) != -1 && bytes_read != 0) 
        {
            if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
            {
                return;
            }

            if (bytes_read == 0 || bytes_read == -1) 
            {
                //close_backend_socket(closure->backend_handler);
                close_backend_socket(closure->backend_handler);
                close_client_socket(self);
                return;
            }

            //write(closure->backend_handler->fd, read_buffer, bytes_read);
            if(make_request(read_buffer, closure->backend_addr))
            {
                write(closure->backend_handler->fd, read_buffer,sizeof(read_buffer));
                //write_to_client(closure->backend_handler, read_buffer, bytes_read);
            }
            else
            {
                return;
            }

        }
    }

    if ((events & EPOLLERR) | (events & EPOLLHUP) | (events & EPOLLRDHUP)) 
    {
        close_backend_socket(closure->backend_handler);
        close_client_socket(self);
        return;
    }

}



struct epoll_event_handler* create_client_socket_handler(int client_socket_fd, int epoll_fd, char*backend_host)
{
    
    make_socket_non_blocking(client_socket_fd);

    struct client_socket_event_data* closure = malloc(sizeof(struct client_socket_event_data));

    struct epoll_event_handler* result = malloc(sizeof(struct epoll_event_handler));
    result->fd = client_socket_fd;
    result->handle = handle_client_socket_event;
    result->closure = closure;

   
    closure->write_buffer = NULL;
    closure->backend_addr = backend_host;

    // add epoll event
    epoll_add_handler(epoll_fd, result, EPOLLIN | EPOLLRDHUP | EPOLLET | EPOLLOUT);

    return result;
}
