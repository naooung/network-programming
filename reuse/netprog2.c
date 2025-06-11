#include "netprog2.h"

void errquit(const char *msg) { perror(msg); exit(1); }

int tcp_connect(int af, const char *servip, unsigned short port) {
    struct sockaddr_in servaddr;
    int  s;

    if ((s = socket(af, SOCK_STREAM, 0)) < 0) return -1;

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = af;
    inet_pton(AF_INET, servip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    if (connect(s, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        return -1;
    return s;
}

int tcp_listen(int host, int port, int backlog) {
    int sd;
    struct sockaddr_in servaddr;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) errquit("socket fail");
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(host);
    servaddr.sin_port = htons(port);
    if (bind(sd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        errquit("bind fail");
    listen(sd, backlog);
    return sd;
}

int udp_client_socket(const char *servip, unsigned short port,
        struct sockaddr_in *pservaddr) {
    int  s;

    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return -1;

    bzero((char*)pservaddr, sizeof(struct sockaddr_in));
    pservaddr->sin_family = AF_INET;
    inet_pton(AF_INET, servip, &(pservaddr->sin_addr));
    pservaddr->sin_port = htons(port);

    return s;
}

int udp_server_socket(int host, unsigned short port) {
    struct sockaddr_in servaddr;
    int  s, addrlen = sizeof(servaddr);

    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return -1;

    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(host);
    servaddr.sin_port = htons(port);

    if (bind(s, (struct sockaddr*)&servaddr, addrlen) < 0) return -1;
    return s;
}