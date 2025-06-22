#include "netprog.h"
#define MAXLINE 511

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    int s, nbyte, addrlen = sizeof(servaddr);
    char buf[MAXLINE + 1];

    if (argc != 3) {
        printf("usage: %s ip_addr port\n", argv[0]);
        exit(0);
    }
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        errquit("socket fail");

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    servaddr.sin_port = htons(atoi(argv[2]));
    printf("입력: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL)
        errquit("fgets 실패");

    if (sendto(s, buf, strlen(buf), 0, (struct sockaddr*)&servaddr,
                sizeof(servaddr)) < 0)
        errquit("sendto  fail");

    if ((nbyte = recvfrom(s, buf,MAXLINE, 0, (struct sockaddr*)&servaddr,
                    &addrlen)) < 0)
        errquit("recvfrom fail");

    buf[nbyte] = 0;
    printf("수신: %s", buf);

    close(s);
}