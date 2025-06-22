#include "netprog.h"
#define MAXLINE 511

struct calcmsg {
    char type;
    char status;
    char op;
    char padding;
    int32_t op1, op2, result;
};

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    int s, nbyte, addrlen;
    char buf[MAXLINE+1];

    if (argc != 3){
        printf("usage: %s ip_addr port\n", argv[0]);
        exit(0);
    }

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        errquit("socket fail");

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    servaddr.sin_port = htons(atoi(argv[2]));

    char op;
    int32_t op1, op2;
    printf("입력 : ");

    scanf("%d %c %d", &op1, &op, &op2);
    struct calcmsg reqmsg;
    reqmsg.op = op;
    reqmsg.op1 = htonl(op1);
    reqmsg.op2 = htonl(op2);

    if (sendto(s, &reqmsg, sizeof(struct calcmsg), 0,
                (struct sockaddr*)&servaddr, sizeof(servaddr)) <0)
        errquit("sendto fail");

    struct calcmsg rspmsg;
    if((nbyte = recvfrom(s, &rspmsg, sizeof(struct calcmsg), 0,
                (struct sockaddr*)&servaddr, &addrlen)) < 0)
        errquit("recvfrom fail");

    if (rspmsg.status == 0){
        int32_t result = ntohl(rspmsg.result);
        printf("%d %c %d = %d\n", op1, op, op2, result);
    }

    close(s);
}