#include "srvsocket.hpp"


SrvSocket::SrvSocket(int domain, int type, int protocol, sa_family_t sin_family, in_port_t port, in_addr_t in_addr, int mconn) : Socket(domain, type, protocol, sin_family, port, in_addr), maxconn(mconn)
{
    bind_fd();
    listen_on_port();
}

void SrvSocket::bind_fd() {
    if (bind(get_sockfd(), (struct sockaddr*)get_addr(), sizeof(*get_addr())) < 0) {
        ERR_EXIT("Bind");
    }
}

void SrvSocket::listen_on_port() {
    if(listen(get_sockfd(), maxconn) < 0) {
        ERR_EXIT("Listen");
    }
}

void SrvSocket::do_service() {
    int new_socket;
    long valread;
    int addrlen = sizeof(*get_addr());
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    while (1)
    {
        std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
        std::cout << "size: " << sizeof(*get_addr()) << std::endl;
        if ((new_socket = accept(get_sockfd(), (struct sockaddr *)get_addr(), (socklen_t*)&addrlen))<0)
        {
            ERR_EXIT("Accept");
        }
        
        char buffer[30000] = {0};
        valread = read(new_socket , buffer, 30000);
        printf("%s\n",buffer );
        write(new_socket , hello , strlen(hello));
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
}

