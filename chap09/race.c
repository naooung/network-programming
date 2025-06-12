#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define MAX_THR 2

void *thrfunc(void *arg);
void prn_data(pthread_t me);

pthread_t who_run = 0; // 공유 데이터
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; // 뮤텍스 초기화

int main(int argc, char **argv) {
    pthread_t tid[MAX_THR];
    int i, status;

    // 스레드를 생성하고 생성한 스레드가 thrfunc 함수를 실행
    for (i = 0; i < MAX_THR; i++) {
        if ((status = pthread_create(&tid[i], NULL, &thrfunc, NULL)) != 0) {
            printf("thread create error: %s\n", strerror(status));
            exit(0);
        }
    }

    pthread_join(tid[0], NULL);
    return 0;
}

// 공유 데이터 값을 변경하는 함수를 실행하기 전에 뮤텍스를 잠금 & 해제
void *thrfunc(void *arg) {
    
    while (1) {
        pthread_mutex_lock(&m);     // 뮤텍스 잠금
        prn_data(pthread_self());
        pthread_mutex_unlock(&m);   // 뮤텍스 해제
    }
    return NULL;
}

// 스레드 아이디를 인자로 받고 출력하는 함수
void prn_data(pthread_t me) {
    who_run = me;

    if (who_run != pthread_self()) 
        printf("Error: %lu 스레드 실행 중 who_run = %lu\n", me, who_run);
    else
        printf("%lu\n", who_run);
    
        who_run = 0;
}