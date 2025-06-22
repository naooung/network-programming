#include "netprog.h"
#define MAXLINE  511

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr, cliaddr;
    int s, nbyte, addrlen = sizeof(cliaddr);
    char buf[MAXLINE+1];

    if (argc != 2) {
        printf("usage: %s port\n", argv[0]);
        exit(0);
    }

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        errquit("socket fail");

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    if (bind(s, (struct sockaddr*)&servaddr, addrlen) < 0)
        errquit("bind fail");

    while (1) {
        puts("Server: waiting request.");
        nbyte = recvfrom(s, buf, MAXLINE, 0,
                    (struct sockaddr*)&cliaddr, &addrlen);
        if (nbyte < 0)
            errquit("recvfrom fail");
        buf[nbyte] = 0;
        printf("%d byte recvd: %s\n", nbyte, buf);

        if (sendto(s, buf, nbyte, 0, (struct sockaddr*)&cliaddr, addrlen) < 0)
            errquit("sendto fail");
        puts("sendto complete");
    }
    return 0;
}