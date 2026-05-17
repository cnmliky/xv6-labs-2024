#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int p1[2], p2[2];
    char buf[1];

    if (pipe(p1) < 0 || pipe(p2) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0) { // 子进程
        close(p1[1]);
        close(p2[0]);
        if (read(p1[0], buf, 1) != 1) {
            exit(1);
        }
        printf("%d: received ping\n", getpid());
        write(p2[1], "x", 1);
        exit(0);
    } else { // 父进程
        close(p1[0]);
        close(p2[1]);
        write(p1[1], "x", 1);
        if (read(p2[0], buf, 1) != 1) {
            exit(1);
        }
        printf("%d: received pong\n", getpid());
        exit(0);
    }
}
