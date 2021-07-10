#include "srvsocket.hpp"

int main()
{
    // 初始化服务器
    SrvSocket srv(AF_INET, SOCK_STREAM, 0, AF_INET, PORT, INADDR_ANY, 10);
    std::cout << "<-----------Waiting for a connection!----------->\n" << std::endl;
    srv.do_service();
    return 0;
}