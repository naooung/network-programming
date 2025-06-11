#include "netprog2.h"
#define MAXLINE 1023

int main(int argc, char *argv[]) {
    int send_s, recv_s, n, len;
    pid_t pid;
    unsigned int yes = 1;
    struct sockaddr_in mcast_group;
    struct ip_mreq mreq; 
    char name[10];

    if (argc != 4) {
        printf("사용법: %s mcast_addr port name\n", argv[0]);
        exit(0);
    }
    // 받은 인자를 사용자 이름 name에 저장
    sprintf(name, "[%s]", argv[3]); 

    // 멀티캐스트 그룹 주소 정보 등록
    memset(&mcast_group, 0, sizeof(mcast_group));
    mcast_group.sin_family = AF_INET;
    mcast_group.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &mcast_group.sin_addr);

    // 멀티캐스트 메세지를 수신하는 소켓 생성
    if ((recv_s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        errquit("receive socket");

    // 멀티캐스트 그룹 주소로 수신을 받을 수 있게 설정
    mreq.imr_multiaddr = mcast_group.sin_addr;
    mreq.imr_interface.s_addr = htol(INADDR_ANY);
    
    // 멀티캐스트 그룹 가입
    if (setsockopt(recv_s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
        errquit("add membership");

    // 소켓 재사용 옵션 지정
    if (setsockopt(recv_s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
        errquit("set reuse option");

    // 소켓 bind
    if (bind(recv_s, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < 0)
        errquit("bind receive socket");

    // 멀티캐스트 메세지를 송신하는 소켓 생성
    if ((send_s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        errquit("send socket");

    // fork 시도
    if ((pid = fork()) < 0)
        errquit("fork");

    // 자식 프로세스: 메세지 수신
    else if (pid == 0) {
        struct sockaddr_in from;
        char message[MAXLINE + 1];
        printf("receiving message...\n");

        while (1) {
            len = sizeof(from);
            if ((n = recvfrom(recv_s, message, MAXLINE, 0, (struct sockaddr*)&from, &len)) < 0)
                errquit("recvfrom");

            message[n] = 0;
            printf("Received Message: %s\n", message);
        }
    }

    // 부모 프로세스: 키보드 입력 및 메세지 송신
    else {
        char message[MAXLINE + 1], line[MAXLINE + 1];

        while (fgets(message, MAXLINE, stdin) != NULL) {
            sprintf(line, "%s %s", name, message);
            len = strlen(line);

            if (sendto(send_s, line, strlen(line), 0, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < len)
                errquit("sendto");
        }
    }
    return 0;
}