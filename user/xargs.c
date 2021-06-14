// shell 命令 xargs
// 这个命令会有两种情况， 有-n的情况和没有-n 的情况
// exec 系统调用会覆盖掉
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
//#include "kernel/fs.h"
typedef int pid_t;

void xarg(char *argv[]) {
    // 创建子进程，程序在子进程里面实现，父进程挂起等待子进程的结束
    pid_t pid;
    if ((pid = fork()) < 0) {
        printf("xargs: fork error...\n");
        exit(1);
    }
    if (pid > 0) {
        wait((int*)0);
        exit(0);
    }
}