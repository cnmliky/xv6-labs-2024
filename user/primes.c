#include "kernel/types.h"
#include "user/user.h"

// 屏蔽编译器的无限递归警告
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winfinite-recursion"

void run_sieve(int p[2]) {
    int prime;
    close(p[1]); // 关闭左侧写端

    if (read(p[0], &prime, sizeof(int)) != sizeof(int)) {
        close(p[0]);
        exit(0);
    }

    printf("prime %d\n", prime);

    int next_p[2];
    if (pipe(next_p) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    if (fork() == 0) {
        // 【关键修复】：子进程不再需要左侧的读端了
        close(p[0]); 
        run_sieve(next_p);
    } else {
        close(next_p[0]); // 父进程不需要右侧读端
        int n;
        while (read(p[0], &n, sizeof(int)) == sizeof(int)) {
            if (n % prime != 0) {
                write(next_p[1], &n, sizeof(int));
            }
        }
        close(p[0]);
        close(next_p[1]);
        wait(0);
        exit(0);
    }
}
#pragma GCC diagnostic pop

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        run_sieve(p);
    } else {
        close(p[0]);
        // 实验要求覆盖到 280
        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        wait(0);
    }
    exit(0);
}
