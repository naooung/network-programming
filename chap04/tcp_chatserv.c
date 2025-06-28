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
void removeClient(int s); // 채팅 탈퇴 처리 함수
int getmax();
int tcp_listen(int host, int port, int backlog); // 소켓 생성과 listen() 처리

int main(int argc, char *argv[]) {
    struct sockaddr_in cliaddr;
    char buf[MAXLINE+1];
    int i, j, nbyte, accp_sock, addrlen = sizeof(struct sockaddr_in);
    fd_set read_fds; // 구조체에 I/O 변화를 감지할 소켓

    if(argc != 2) {
        printf("사용법 : %s port\n", argv[0]);
        exit(0);
    }

    listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);

    while(1) {
        FD_ZERO(&read_fds); // 감시할 fd 초기화
        FD_SET(listen_sock, &read_fds); // listen 소켓 추가
        for(i = 0; i < num_chat; i++)
            FD_SET(clisock_list[i], &read_fds); // 연결된 클라이언트 소켓 추가

        maxfdp1 = getmax() + 1; // 감시할 파일 디스크립터의 개수가 아니라 범위를 넘겨주기 위해 +1

        puts("wait for client");
        if(select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0) // maxfdp1 비트만큼 검사
            errquit("select fail");

        // -------- 연결용 소켓 변화 = 새 클라이언트 연결 처리 ---------
        if(FD_ISSET(listen_sock, &read_fds)) {
            accp_sock = accept(listen_sock, (struct sockaddr*)&cliaddr, &addrlen);
            if(accp_sock == -1)
                errquit("accept fail");

            addClient(accp_sock, &cliaddr);
            send(accp_sock, START_STRING, strlen(START_STRING), 0);
            printf("%d번째 사용자 추가.\n", num_chat);
        }

        // -------- 기존 클라이언트 변화--------
        for(i = 0; i < num_chat; i++) {
            if(FD_ISSET(clisock_list[i], &read_fds)) {
                nbyte = recv(clisock_list[i], buf, MAXLINE, 0);

                // nbyte가 0보다 작으면 클라이언트 탈퇴
                if(nbyte <= 0) {
                    removeClient(i);
                    continue;
                }

                // 탈퇴 입력 확인
                buf[nbyte] = 0;
                if(strstr(buf, EXIT_STRING) != NULL) {
                    removeClient(i);
                    continue;
                }

                // 모든 클라이언트에게 수신한 메세지 전송
                for(j = 0; j < num_chat; j++)
                    send(clisock_list[j], buf, nbyte, 0);
                printf("%s\n", buf);
            }
        }
    }
    return 0;
}

// ------ 새 클라이언트 ip 출력 +  배열에 추가 ------
void addClient(int s, struct sockaddr_in *newcliaddr) {
    char buf[20];
    inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf));
    printf("new client: %s\n", buf, ntohs(newcliaddr->sin_port));

    // 소켓 배열 맨 뒤에 추가하여 채팅 클라이언트 목록에 추가
    clisock_list[num_chat] = s;
    num_chat++;
}

// ------ 클라이언트 소켓을 닫기 + 배열 중간 삭제시 맨 마지막 원소와 변경 ------
void removeClient(int s) {
    close(clisock_list[s]);
    if(s != num_chat - 1)
        clisock_list[s] = clisock_list[num_chat - 1];
    num_chat--;
    printf("채팅 참가자 1명 탈퇴. 현재 참가자 수 = %d\n", num_chat);
}

// ------- select에 필요한 최대 비트 계산 = 연결용 소켓 vs 클라이언트 배열 중 큰 값 리턴 ------
int getmax() {
    int max = listen_sock;
    int i;
    for(i = 0; i < num_chat; i++)
        if(clisock_list[i] > max)
            max = clisock_list[i];
    return max;
}