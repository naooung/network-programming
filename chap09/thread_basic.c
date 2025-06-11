#include "netprog2.h"
#include <pthread.h>

void *thrfunc(void *arg);
char who[10];

int main(int argc, char **argv) {
    int status;
    pthread_t tid;
    pid_t pid;

    // fork 실행한 결과를 who에 저장
    if ((pid = fork()) == 0)
        sprintf(who, "child");
    else
        sprintf(who, "parent");

    // main thread가 실행하는 내용
    printf("(%s's main) Process ID = %d\n", who, getpid());
    printf("(%s's main) Init thread ID = %lu\n", who, pthread_self());

    // 스레드 시작 함수 지정
    status = pthread_create(&tid, NULL, &thrfunc, NULL);
    if (status != 0) {
        printf("thread create error: %s\n", strerror(status));
        exit(0);
    }

    // tid가 종료될 때까지 기다린 뒤에 종료
    pthread_join(tid, NULL);
    printf("\n(%s) [%lu] 스레드가 종료했습니다\n", who, tid);

    return 0;
}

// 새 스레드가 실행하는 내용
void *thrfunc(void *arg) {
    printf("(%s's main) Process ID = %d\n", who, getpid());
    printf("(%s's main) Init thread ID = %lu\n", who, pthread_self());
}

/*
 * 부모 프로세스 (PID A)
 *  ├── 메인 스레드 (T1)
 *  └── 새 스레드 (T2)

 *  자식 프로세스 (PID B)
 *  ├── 메인 스레드 (T3)
 *  └── 새 스레드 (T4)
 */