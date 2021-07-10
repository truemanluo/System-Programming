#include "tinyhttp.hpp"
#include "socket.hpp"

class SrvSocket : public Socket
{
private:
    int maxconn;
public:
    SrvSocket(int domain, int type, int protocol, sa_family_t sin_family, in_port_t port, in_addr_t in_addr, int mconn);
    // ~SrvSocket();

    void bind_fd();
    void listen_on_port();
    void do_service();
};
