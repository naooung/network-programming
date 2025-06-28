#include "netprog2.h"
#define NAME_LEN 20
#define MAXLINE 1000

char *EXIT_STRING = "exit";

int main(int argc, char *argv[]) {
    char bufall[MAXLINE + NAME_LEN], *bufmsg; // 닉네임 + 메세지
    int maxfdp1, s, namelen;
    fd_set read_fds;

    if (argc != 4) {
        printf("사용법 : %s server_ip port name\n", argv[0]);
        exit(0);
    }

    // ------ bufall 초기화 ------
    sprintf(bufall, "[%s] :", argv[3]); // bufall에 "[닉네임]:" 저장
    namelen = strlen(bufall); // 현재 길이를 namelen으로 저장
    bufmsg = bufall + namelen; // bufmsg 포인터의 위치 설정

    // ------ 소켓 생성 후 연결 요청 함수 호출 ------
    s = tcp_connect(AF_INET, argv[1], atoi(argv[2]));
    if (s == -1)
        errquit("tcp_connect fail");

    puts("서버에 접속되었습니다.");
    maxfdp1 = s + 1; // 확인할 서버 디스크립터 + 1 하여 범위 확인

    // ------ 키보드나 서버 입력이 있는지 감시하기 위한 설정 -------
    FD_ZERO(&read_fds);
    while (1) {
        FD_SET(0, &read_fds);
        FD_SET(s, &read_fds); // 서버 소켓 변화 감지 설정

        // 2개의 입력을 대기: 키보드(3)  서버 연결(5)
        if (select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0)
            errquit("select fail");

        // 소켓으로부터 메세지 수신 read()
        if (FD_ISSET(s, &read_fds)) {
            int nbyte;
            if ((nbyte = recv(s, bufmsg, MAXLINE, 0)) > 0) {
                bufmsg[nbyte] = 0;
                printf("%s\n", bufmsg);
            }
        }
        
        // 키보드 입력 처리 send()
        else if (FD_ISSET(0, &read_fds)) {
            if (fgets(bufmsg, MAXLINE, stdin)) {
                if (send(s, bufall, namelen + strlen(bufmsg), 0) < 0) // 닉네임이 붙은 전체를 서버로 전송
                    puts("Error : Write error on socket.");

                if (strstr(bufmsg, EXIT_STRING) != NULL) { // 종료 문자 입력시
                    puts("Good bye.");
                    close(s);
                    exit(0);
                }
            }
        }
    }
}
