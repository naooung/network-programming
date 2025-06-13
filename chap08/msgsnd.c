#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFSZ 512

typedef struct _msg {
    long msg_type;
    char msg_text[BUFSZ];
} msg_t;

int message_send(int qid, long type, const char* text);

int main(int argc, char **argv) {

    if (argc != 2) {
        prinbtf("Usage: %s msqkey\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 메세지큐의 고유 식별자 설정
    key_t key = atoi(argv[1]);

    // key를 기반으로 메세지큐 생성 후 id 리턴
    int qid = msgget(key, IPC_CREAT | 0600);
    if (pid < 0) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    char text[BUFSZ];
    put("Enter message to post: ");
    fgets(text, BUFSZ, stdin);
    message_send(qid, 3, text);

    put("Enter message to post: ");
    fgets(text, BUFSZ, stdin);
    message_send(qid, 2, text);

    put("Enter message to post: ");
    fgets(text, BUFSZ, stdin);
    message_send(qid, 1, text);

    return 0;
}

int message_send(int qid, long type, const char* text) {
    msg_t pmsg;
    pmsg.msg_type = type;

    int len = strlen(text);
    strncpy(pmsg.msg_text, text, len);

    pmsg.msg_text[len] = 0;
    msgsnd(qid, &pmsg, len, 0);

    return 0;
}