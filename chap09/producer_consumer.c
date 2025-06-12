#include "netprog2.h"
#include <pthread.h>

#define NUM_CONSUMER 10

void *consumer_func(void *arg);
void *producer_func(void *arg);

int resource_count = 0;
pthread_mutex_t resource_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t resource_cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[]) {
    int i, id[NUM_CONSUMER];
    pthread_t consumer_thread[NUM_CONSUMER], producer_thread;

    // 소비자 스레드를 생성하고 id값을 인자로 하여 consumer_func()으로 자원 소비 실행
    for (i = 0; i < NUM_CONSUMER; i++) {
        id[i] = i;
        pthread_create(&consumer_thread, NULL, &consumer_func, &id[i]);
    }

    // 생산자 스레드 생성하고 producer_func()으로 자원 생성 실행
    pthread_create(&producer_thread, NULL, &producer_func, NULL);

    // 생상자 스레드가 종료될 때까지 대기 설정
    pthread_join(producer_func, NULL);

    return 0;
}

// 지원을 소비하는 함수 (자원이 없으면 wait, 자원이 생기면 자원 소비)
void *consumer_func(void *arg) {
    int trial = 0, success = 0, id = *((int*)arg);
    
    while (1) {
        pthread_mutex_lock(&resource_lock);

        while (resouce_count == 0)
            pthread_cond_wait(&resource_cond, &resource_lock);
        trial++;

        if (resource_count > 0) {
            resouce_count--;
            success++;
        }

        pthread_mutex_unlock(&resource_lock);
        printf("%2d: trial = %d success = %d\n", id, trial, success);
    }

    return NULL;
}

// 자원을 생산하고 broadcast로 소비자에게 알림을 보내는 함수
void *producer_func(void *arg) {
    
    while (1) {
        pthread_mutex_lock(&resource_lock);
        resource_count++;

        pthread_cond_broadcast(&resource_cond);
        pthread_mutex_unlock(&resource_lock);
    }

    return NULL;
}