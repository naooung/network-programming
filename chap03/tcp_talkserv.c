#include "netprog.h"
char* EXIT_STRING = "exit";
int recv_and_print(int sd);
int input_and_send(int sd);
#define MAXLINE 511

int main(int argc, char *argv[]) {
    struct sockaddr_in cliaddr, servaddr;
    int listen_sock, accp_sock;
    int addrlen = sizeof(cliaddr);
    pid_t pid;

    if (argc != 2) {
        printf("사용법: %s port\n", argv[0]);
        exit(0);
    }

    if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket fail");
        exit(0);
    }

    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("bind fail");
        exit(0);
    }

    puts("서버가 클라이언트를 기다리고 있습니다.");
    listen(listen_sock, 1);

    if ((accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen)) < 0) {
        perror("accept fail");
        exit(0);
    }
    puts("클라이언트가 연결되었습니다.");

    if ((pid = fork()) > 0)
        input_and_send(accp_sock);
    else if (pid == 0) {
        char line[80];
        sprintf(line, "%d", accp_sock);
        execl("tcp_talkexec", "tcp_talkexec", line);
        // recv_and_print(accp_sock);
    }

    close(listen_sock);
    close(accp_sock);
    return 0;
}

int input_and_send(int sd) {
    char buf[MAXLINE + 1];
    int nbyte;
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        nbyte = strlen(buf);
        write(sd, buf, nbyte);

        if (strstr(buf, EXIT_STRING) != NULL) {
            puts("Good bye.");
            close(sd);
            exit(0);
        }
    }
    return 0;
}

int recv_and_print(int sd) {
    char buf[MAXLINE + 1];
    int nbyte;
    while (1) {
        if ((nbyte = read(sd, buf, MAXLINE)) < 0) {
            perror("read fail");
            close(sd);
            exit(0);
        }
        buf[nbyte] = 0;

        if (strstr(buf, EXIT_STRING) != NULL)
            break;
        printf("%s", buf);
    }
    return 0;
}