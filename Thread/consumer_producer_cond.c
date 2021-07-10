#include "threadsutils.h"

#define ERR_EXIT(m) \
        do \
        { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while (0)

#define CONSUMERS 2 // 消费者数量
#define PRODUCERS 4 // 生产者数量

void* consume(void *args);
void* produce(void *args);

pthread_mutex_t mutex; 
pthread_cond_t cond;
pthread_t g_threads[CONSUMERS+PRODUCERS];

int nready = 0; // 缓冲区产品数量


int main()
{
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    int i;
    // 创建消费者进程
    for (i = 0; i < CONSUMERS; ++i) {
        pthread_create(g_threads+i, NULL, consume, NULL);
    }

    // 创建生产者进程
    for (; i < CONSUMERS+PRODUCERS; ++i) {
        pthread_create(g_threads+i, NULL, produce, NULL);
    }

    // 进程回收
    for (i = 0; i < PRODUCERS+CONSUMERS; ++i) {
        pthread_join(g_threads[i], NULL);
    }

    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    sleep(1);


    return 0;
}

void* consume(void *args) {
    while (1)
    {
        pthread_mutex_lock(&mutex);
        // 没有产品则等待
        while (nready == 0)
        {
            printf("%ld Waiting for a condition...\n", pthread_self());
            pthread_cond_wait(&cond, &mutex);
        }
        // 消费一个产品
        --nready;
        printf("%ld End the waiting for a condition...\n", pthread_self());    
        pthread_mutex_unlock(&mutex);
        sleep(1);         
    }
    return NULL;
}


void* produce(void *args) {
    while (1)
    {
        pthread_mutex_lock(&mutex);
        printf("%ld Begin producing...\n", pthread_self());
        ++nready;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
    
    return NULL;

}