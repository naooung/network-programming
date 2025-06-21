#include "netprog.h"
#define MAXLINE  127

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    int  s, nbyte;
    char buf[MAXLINE+1];

    struct sockaddr_in cliaddr;
    int addrlen = sizeof(cliaddr);

    if (argc != 3) {
        printf("usage: %s ip_address port_number\n", argv[0]);
        exit(0);
    }

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        errquit("socket fail");

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_aton(argv[1], &servaddr.sin_addr);
    servaddr.sin_port = htons(atoi(argv[2]));

    if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        errquit("connect fail");

    getsockname(s, (struct sockaddr*)&cliaddr, &addrlen);
    char IPaddr[20];
    inet_ntop(AF_INET, &cliaddr.sin_addr, IPaddr, sizeof(IPaddr));
    printf("client %s:%u\n", IPaddr, ntohs(cliaddr.sin_port));

    printf("입력: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL)
        errquit("fgets 실패\n");
    nbyte = strlen(buf);

    if (write(s, buf, nbyte) < 0)
        errquit("write fail");

    printf("수신: ");
    if ((nbyte = read(s, buf, MAXLINE)) < 0)
        errquit("read fail");

    buf[nbyte] = 0;
    printf("%s", buf);

    close(s);
    return 0;
}