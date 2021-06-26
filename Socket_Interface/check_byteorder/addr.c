#include <stdio.h>
#include <arpa/inet.h>

int main()
{
    unsigned long addr = inet_addr("192.168.53.242");
    printf("addr = %u\n", ntohl(addr));

    // 

    return 0;
}