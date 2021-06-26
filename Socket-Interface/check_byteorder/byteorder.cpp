#include <iostream>
using namespace std;

int main()
{
    unsigned int num = 0x12345678;
    unsigned char *p = (unsigned char*)&num;
    for (int i = 0; i < 4; ++i) {
        cout << std::hex << (unsigned int)p[i] << endl;
    }
    return 0;
}