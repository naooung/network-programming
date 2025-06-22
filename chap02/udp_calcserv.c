#include "netprog.h"
#define MAXLINE 511

struct calcmsg {
    char type;
    char status;
    char op;
    char padding;
    int32_t op1, op2, result;
};

int main(int argc, char *argv[]){
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

    while(1) {
        puts("Server: waiting request.");
        struct calcmsg reqmsg;

        recvfrom(s, &reqmsg, sizeof(struct calcmsg), 0,
                (struct sockaddr*)&cliaddr, &addrlen);

        char op;
        int32_t op1, op2, result;
        op1 = ntohl(reqmsg.op1);
        op2 = ntohl(reqmsg.op2);
        op = ntohl(reqmsg.op);

        switch(reqmsg.op) {
            case '+': result = op1 + op2; break;
            case '-': result = op1 - op2; break;
            case '*': result = op1 * op2; break;
            case '/': result = op1 / op2; break;
        }

        struct calcmsg rsqmsg;
        rsqmsg.status = 0;
        rsqmsg.result = htonl(result);

        if (sendto(s, &rsqmsg, sizeof(struct calcmsg), 0,
                    (struct sockaddr*)&cliaddr, addrlen) < 0)
            errquit("send to fail");
        puts("sendto complete");
    }
}