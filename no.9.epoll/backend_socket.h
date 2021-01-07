struct backend_socket_event_data 
{
    struct epoll_event_handler* client_handler;
};


extern void handle_backend_socket_event(struct epoll_event_handler* self, uint32_t events);

extern void close_backend_socket(struct epoll_event_handler*);

extern struct epoll_event_handler* create_backend_socket_handler(int epoll_fd, int backend_socket_fd);
