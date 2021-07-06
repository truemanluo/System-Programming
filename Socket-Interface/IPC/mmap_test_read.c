#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#define ERR_EXIT(m) \
        do  \
        {   \
            perror(m);  \
            exit(EXIT_FAILURE); \
        } while(0);

typedef struct stu
{
    char name[4];
    int age;
} STU;



int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        ERR_EXIT("usage");
    }

    int fd;
    fd = open(argv[1], O_RDONLY);


    STU* p;
    p = (STU*)mmap(NULL, sizeof(STU)*5, PROT_READ, MAP_SHARED, fd, 0);
    if (p == NULL) {
        ERR_EXIT("mmap");
    }

    // 从共享内存中读入信息
    for (int i = 0; i < 5; ++i) {
        printf("name = %s, age = %d\n", (p+i)->name, (p+i)->age);
    }
    printf("初始化完毕！\n");
    munmap(p, sizeof(STU)*5);
    return 0;
}