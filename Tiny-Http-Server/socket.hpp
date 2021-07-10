#ifndef SOCKH
#define SOCKH

#include "tinyhttp.hpp"

class Socket
{
private:
    int sockfd;
    struct sockaddr_in servaddr;

public:
    Socket(int domain, int type, int protocol, sa_family_t sin_family, in_port_t port, in_addr_t in_addr);
    // ~Socket();

    int get_sockfd();
    struct sockaddr_in* get_addr();
};



#endif