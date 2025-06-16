#include "netprog2.h"
#include <pthread.h>

#define MAXLINE 511
#define MAX_SOCK 100

void addClient(int sock);
void removeClient(int sock);
void *recv_and_send(void *arg);

int socklist[MAX_SOCK];
int num_chat = 0;
// 멀티스레드에서 socklist 접근 시 동기화 문제 방지용 뮤텍스 
pthread_mutex_t socklist_lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
    struct sockaddr_in cliaddr;
    int listen_sock, sock, addrlen = sizeof(struct sockaddr_in);

    if (argc != 2) {
        printf("Usage: %s port\n", argv[0]);
        exit(1);
    }

    listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);

    // 클라이언트 접속 시 소켓을 받은 후 addClient() 호출
    while (1) {
        sock = accept(listen_sock, (struct sockaddr*)&cliaddr, &addrlen);
        addClient(sock);
    }
    return 0;
}

// 새 클라이언트 소켓을 인자로 받아 socklist 배열에 추가하는 함수 (동기화 문제 해결을 위해 뮤텍스 사용)
void addClient(int sock) {
    pthread_t tid;

    pthread_mutex_lock(&socklist_lock);
    socklist[num_chat] = sock;
    num_chat++;
    pthread_mutex_unlock(&socklist_lock);

    pthread_create(&tid, NULL, recv_and_send, &sock);
}

// socklist 배열에서 제거하는 함수 (뮤텍스 사용)
void removeClient(int sock) {
    int i;

    pthread_mutex_lock(&socklist_lock);

    for (i = 0; i < num_chat; i++) {
        if (socklist[i] == sock) {
            if (i != num_chat - 1)
                socklist[i] = socklist[num_chat - 1];
            num_chat--;
        }
    }
    pthread_mutex_unlock(&socklist_lock);
}

// 클라이언트 추가로 인해 스레드를 생성했을 때 실행되는 함수
// 클라이언트의 메세지를 read, socklist 배열에 write 작업 수행하는 함수 (뮤텍스 사용)
void *recv_and_send(void *arg) {
    int i, nbyte, sock = *((int*)arg);
    char buf[MAXLINE + 1];

    printf("thread %lu started,\n", pthread_self());

    while (1) {
        nbyte = read(sock, buf, MAXLINE);

        if (nbyte <= 0) {
            close(sock);
            removeclient(sock);
            break;
        }

        buf[nbyte] = 0;

        pthread_mutex_lock(&socklist_lock);
        for (i = 0; i < num_chat; i++)
            write(socklist[i], buf, nbyte);
        pthread_mutex_unlock(&socklist_lock);
    }

    printf("thread %lu stopped.\n", pthread_self());
}