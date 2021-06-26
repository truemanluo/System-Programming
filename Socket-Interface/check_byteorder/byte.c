#include <stdio.h>

void byteorder()
{
    union Byte
    {
        short value;
        char chrs[sizeof(short)];
    } byte;
    
    byte.value = 0x1234;
    if (byte.chrs[0] == 0x12 && byte.chrs[1] == 0x34) {
        printf("Big endian.\n");
    }
    
    else if (byte.chrs[0] == 0x34 && byte.chrs[1] == 0x12) {
        printf("Little endian.\n");
    }

    else {
        printf("Unknow.");
    }
}

int main()
{
    unsigned int num = 0x12345678;
    unsigned char *p = (unsigned char*)&num;
    byteorder();
    return 0;
}