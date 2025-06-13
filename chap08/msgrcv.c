#include "netprog2.h"
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFSZ 512

typedef struct _msg {
    long msg_type;
    char msg_text[BUFSZ];
} msg_t;

void message_receive(int qid, long type);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s msqkey\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    key_t key = atoi(argv[1]);

    int qid = msgget(key, IPC_CREAT | 0600);
    if (qid < 0) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    message_receive(qid, -3);
    message_receive(qid, -3);
    message_receive(qid, -3);

    msgctl(qid, IPC_RMID, 0);

    return 0;
}

void message_receive(int qid, long type) {
    msg_t rmsg;
    int nbytes = msgrcv(qid, &rmsg, sizeof(rmsg.msg_text), type, 0);

    printf("recv = %d bytes\n", nbytes);
    printf("type = %ld\n", rmsg.msg_type);
    printf("value = %s\n", rmsg.msg_text);
}