#include "netprog.h"
#define MAXLINE 127

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr, cliaddr;
    int listen_sock, accp_sock, addrlen = sizeof(cliaddr), nbyte;
    char buf[MAXLINE+1];

    if (argc != 2) {
        printf("usage: %s port\n", argv[0]);
        exit(0);
    }

    if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        errquit("socket fail");

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    if (bind(listen_sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        errquit("bind fail");

    listen(listen_sock, 5);

    while (1) {
        puts("서버가 연결요청을 기다림..");
        accp_sock = accept(listen_sock, (struct sockaddr*)&cliaddr, &addrlen);
        if (accp_sock < 0)
            errquit("accept fail");

        puts("클라이언트가 연결됨..");
        printf("%s:%u\n", inet_ntoa(cliaddr.sin_addr),
                ntohs(cliaddr.sin_port));
        nbyte = read(accp_sock, buf, MAXLINE);
        write(accp_sock, buf, nbyte);
        close(accp_sock);
    }
    close(listen_sock);
    return 0;
}