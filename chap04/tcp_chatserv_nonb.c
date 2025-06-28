#include <fcntl.h>
#include <errno.h>
#include "netprog2.h"
#define MAXLINE 511
#define MAX_SOCK 1024

char *EXIT_STRING = "exit";
char *START_STRING = "Connected to char_server \n";
int maxfdp1;
int num_chat = 0;
int clisock_list[MAX_SOCK];
int listen_sock;

void addClient(int s, struct sockaddr_in *newcliaddr);
int getmax();
void removeClient(int s); // 채팅 탈퇴 처리 함수

int set_nonblock(int sockfd);
int is_nonblock(int sockfd);

int main(int argc, char *argv[]) {
    struct sockaddr_in cliaddr;
    char buf[MAXLINE+1];
    int i, j, nbyte, count;
    int accp_sock, addrlen = sizeof(struct sockaddr_in);

    if(argc != 2) {
        printf("사용법 : %s port\n", argv[0]);
        exit(0);
    }

    // ------ 소켓 생성 & 넌블록 모드 설정 ------
    listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);
    if (listen_sock == -1)
        errquit("tcp_listen fail");
    if (set_nonblock(listen_sock) == -1)
        errquit("set_nonblock fail");

    while(1) {
        sleep(1);
        putchar('.');
        fflush(stdout);

        // ------ accept(): 오류 발생, 즉시 반환, 새 클라이언트 추가 ------
        accp_sock = accept(listen_sock, (struct sockaddr*)&cliaddr, &addrlen);

        if (accp_sock == -1 && errno != EWOULDBLOCK) // errno가 EWOULBLOCK이 아니면 오류 발생
            errquit("accept fail");
        else if (accp_sock > 0) {
            // 리턴된 연결용 소켓은 넌블록 모드가 아니므로 설정 필요
            if (is_nonblock(accp_sock) != 0 && set_nonblock(accp_sock) < 0)
                errquit("set_nonblock fail");
            addClient(accp_sock, &cliaddr);
            send(accp_sock, START_STRING, strlen(START_STRING), 0);
            printf("%d번째 사용자 추가.\n", num_chat);
        }

        for (i = 0; i < num_chat; i++) {
            errno = 0;
            nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
            if (nbyte == 0) {
                removeClient(i);
                continue;
            }
            else if (nbyte == -1 && errno == EWOULDBLOCK)
                continue;

            if (strstr(buf, EXIT_STRING) != NULL) {
                removeClient(i);
                continue;
            }
            buf[nbyte] = 0;
            for(j = 0; j < num_chat; j++)
                send(clisock_list[j], buf, nbyte, 0);
            printf("%s\n", buf);
        }
    }
}

void addClient(int s, struct sockaddr_in *newcliaddr) {
    char buf[20];
    inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf));
    printf("new client: %s\n", buf, ntohs(newcliaddr->sin_port));
    // 소켓 배열 맨 뒤에 추가하여 채팅 클라이언트 목록에 추가
    clisock_list[num_chat] = s;
    num_chat++;
}

void removeClient(int s) {
    close(clisock_list[s]);
    if(s != num_chat - 1)
        clisock_list[s] = clisock_list[num_chat - 1];
    num_chat--;
    printf("채팅 참가자 1명 탈퇴. 현재 참가자 수 = %d\n", num_chat);
}

int getmax() {
    int max = listen_sock;
    int i;
    for(i = 0; i < num_chat; i++)
        if(clisock_list[i] > max)
            max = clisock_list[i];
    return max;
}

int is_nonblock(int sockfd) {
    int val = fcntl(sockfd, F_GETFL, 0);
    if (val & O_NONBLOCK)
        return 0;
    return -1;
}

int set_nonblock(int sockfd) {
    int val = fcntl(sockfd, F_GETFL, 0);
    if (fcntl(sockfd, F_SETFL, val | O_NONBLOCK) == -1)
        return -1;
    return 0;
}