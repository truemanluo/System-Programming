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
    fd = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, 0666);
    lseek(fd, sizeof(STU)*5-1, SEEK_SET);

    write(fd, "", 1); // 在offset=39处写入字节，前面默认填充，文件大小为40bytes

    STU* p;
    p = (STU*)mmap(NULL, sizeof(STU)*5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == NULL) {
        ERR_EXIT("mmap");
    }

    // 向共享内存中写入信息（由于fd与共享内存建立了映射，实际也是向文件中写入）
    char ch = 'a';
    int age = 22;
    for (int i = 0; i < 5; ++i) {
        memcpy((p+i)->name, &ch, 1);
        (p+i)->age = age;
        // memcpy((p+i)->age, &age, sizeof(age));
        ++ch;
        ++age;
    }
    printf("初始化完毕！\n");
    munmap(p, sizeof(STU)*5);
    return 0;
}