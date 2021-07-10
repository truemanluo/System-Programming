#include "threadsutils.h"

#define ERR_EXIT(m) \
        do \
        { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while (0)

#define N 10 // 产品数量
#define CONSUMERS 1 // 消费者数量
#define PRODUCERS 2 // 生产者数量

void* consume(void *args);
void* produce(void *args);


int buffer[N]; // 产品队列

// 环形缓冲区
size_t in_index = 0; // 开始生产的位置
size_t out_index = 0; // 开始消费的位置

size_t produce_id = 0; // 当前生产的产品id
size_t consume_id = 0; // 当前消费的产品id


pthread_mutex_t mutex; 
pthread_t g_threads[CONSUMERS+PRODUCERS];

sem_t full; // 缓冲区的满槽数目
sem_t empty; // 缓冲区的空槽数目

size_t c_start = 0; // 开始消费的位置
size_t p_start = 0; // 开始生产的位置

int main()
{
    // 初始化信号量
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, N);
    pthread_mutex_init(&mutex, NULL);

    // 初始化产品缓冲区
    for (int i = 0; i < N; ++i) buffer[i] = -1;

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

    // 清空信号量
    sem_destroy(&full);
    sem_destroy(&empty);
    sleep(1);


    return 0;
}

void* consume(void *args) {
    while (1)
    {
        printf("consume waiting!\n");
        sem_wait(&full); // down
        pthread_mutex_lock(&mutex);
        // 消费
        for (int i = 0; i < N; ++i) {
            printf("%02d ", i);
            if (buffer[i] == -1) {
                printf("%s", "null");
            }
            else
                printf("%d", buffer[i]);
            if (i == out_index) {
                printf("\t<--consume");
            }
            printf("\n");
        }
        consume_id = buffer[out_index];
        // 消费产品
        printf("%ld开始消费%lu\n", pthread_self(), consume_id);
        buffer[out_index] = -1;
        out_index = (out_index+1)%N;
        printf("%ld结束消费%lu\n", pthread_self(), consume_id);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty); // up
        sleep(1);
    }
    


    return NULL;
}


void* produce(void *args) {
    while (1)
    {
        printf("produce waiting!\n");
        sem_wait(&empty); // down
        pthread_mutex_lock(&mutex);
        // 生产
            // 先打印当前状态
        for (int i = 0; i < N; ++i) {
            printf("%02d ", i);
            if (buffer[i] == -1) {
                printf("%s", "null");
            }
            else
                printf("%d", buffer[i]);
            if (i == in_index) {
                printf("\t<--produce");
            }
            printf("\n");
        }
            // 生产产品
        printf("%ld开始生产%lu\n", pthread_self(), produce_id);
        buffer[in_index] = produce_id;
        in_index = (in_index+1)%N;
        printf("%ld结束生产%lu\n", pthread_self(), produce_id++);

        pthread_mutex_unlock(&mutex);
        sem_post(&full); // up

    }
    
    return NULL;

}