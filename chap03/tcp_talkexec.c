#include "netprog.h"
char* EXIT_STRING = "exit";
#define MAXLINE 511

int main(int argc, char *argv[]) {
    char buf[MAXLINE + 1];
    int nbyte, sd;

    sd = atoi(argv[1]);

    while (1) {
        if ((nbyte = read(sd, buf, MAXLINE)) < 0) {
            perror("read fail");
            close(sd);
        }
        buf[nbyte] = 0;

        if (strstr(buf, EXIT_STRING) != NULL)
            break;
        printf("%s", buf);
    }
}