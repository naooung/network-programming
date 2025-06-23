#include "netprog.h"
int global_var = 0;

int main(void) {
    pid_t pid;
    int local_var = 0;

    if ((pid = fork()) < 0) {
        printf("fork error\n");
        exit(0);
    }

    else if (pid == 0) {
        global_var++;
        local_var++;
        printf("CHILD pid = %d, parnet's pid = %d\n", getpid(), getppid());
    }

    else {
        sleep(2);
        global_var += 5;
        local_var += 5;
        printf("RARENT: pid = %d, child's pid = %d\n", getpid(), pid);
    }

    printf("\t global var: %d\n", global_var);
    printf("\t local var: %d\n", local_var);
}