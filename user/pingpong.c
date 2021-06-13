#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

typedef int pid_t;  //这里会使用到pid_t

int
main(int argc, char const *argv[])
{
    // 入参检测
    if (argc > 1) {
        fprintf(2, "paramer error...\n");
        exit(1);
    }
    // 创建用来装载的管道
    int father_fd[2];   //父进程发送
    int child_fd[2];    //子进程发送
    pipe(father_fd);
    pipe(child_fd);
    // 创建子进程，方便进行通信
    pid_t pid = fork(); //system call, 创建子进程进行通信
    if(pid < 0) {
        fprintf(2, "fork error...\n");
        exit(1);
    }
    if (pid == 0) {
        // 这里是子进程
        char buf[512];
        int n;
        close(father_fd[1]);
        close(child_fd[0]);
            // 可能是因为read函数是非阻塞的,
            // read为阻塞函数，这样就可以了
        n = read(father_fd[0], buf, sizeof(buf));
        if (n < 0)  {
        fprintf(2, "read error...\n");
        exit(1);
        }
        printf("%d:received %s\n", getpid(), buf);
        close(father_fd[0]);
        char *send = "pong";
        if (write(child_fd[1], send, sizeof(send)) != sizeof(send)) {
            fprintf(2, "write error...\n");
            exit(1);
        }
        //close(child_fd[1]);
    } else {
        // 这里是父进程，这里可以确保父进程会在子进程后面
        char buf[512];
        char *send = "ping";
        close(father_fd[0]);
        close(child_fd[1]);
        if (write(father_fd[1], send, sizeof(send)) != sizeof(send)) {
            fprintf(2, "write error...\n");
            exit(1);
        }
        // 等待子进程退出，
        wait((int*)0);
        close(father_fd[1]);
        int n = read(child_fd[0], buf, sizeof(buf));
        if (n < 0) {
            fprintf(2, "read error...\n");
            exit(1);
        }
        printf("%d: received %s\n", getpid(), buf);
        close(child_fd[0]);
    }
    exit(0);
}
