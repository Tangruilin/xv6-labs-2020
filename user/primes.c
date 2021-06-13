// 实现一个素数筛, 求35以内的素数
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


#define READ_FD    0
#define WRITE_FD    1
typedef int pid_t;

// 主进程使用循环，循环开启子进程，然后每个子进程都是一个素数筛, 参考一个网上的实现思路
// 但是这个思路并没有把并发特性运用的很好
int
main(int argc, char const *argv[])
{
    // 入参检测
    if (argc > 1) {
        fprintf(2, "paramer error...\n");
        exit(1);
    }
    const int numMax = 34;  //z只需要发送2到35的数字
    int num[numMax];
    int fd[2];
    int index = 0;
    // 数据数组初始化
    for (index = 2; index < numMax + 2; index++)
        num[index - 2] = index;
    index -= 2;
    // 管道初始化
    //pipe(fd);   //创建一个管道就可以了，并不需要向回发送数据
    // 素数筛的主要逻辑，但是感觉这个并发方式和不使用并发的差别并不是很大， 但是这个并不是重点，目前不用去花主要功夫琢磨了
    while(index > 0) {
        pipe(fd);
        pid_t pid = fork();
        if (pid < 0) {
            fprintf(2, "fork error...\n");
            exit(1);
        }
        // 父进程，将数据循环地发送到子进程，发送完后关闭描述符然后退出
        if (pid == 0) {
            close(fd[READ_FD]);
            // 将数组中的数据逐一发送过去，一共要发送的数据数目为index个
            // 这里必须使用 < 而不能使用 <= ， 否则因为index每次都是多放了一个的
            for(int i = 0; i < index; i++) {
                write(fd[WRITE_FD], &num[i], sizeof(num[i]));
            }
            // 发送完毕后关掉，这样read那边可以读到EOF
        close(fd[WRITE_FD]);
        wait((int*)0);
        exit(0);    //主线程正常退出
        } else {
            // prime是筛子， temp是用来存数据的
            close(fd[WRITE_FD]);
            int prime = 0;
            int temp = 0;
            index = -1;
            // tip: 当主进程的筛子管道关闭以后，子进程中的read得到的数据自然会变成0
            while(read(fd[READ_FD], &temp, sizeof(temp)) != 0) {
                if(index == -1) {prime = temp; index++;}  //获取筛子
                if(index >= 0) {
                    if (temp % prime != 0)  num[index++] = temp;    //被筛选出来没问题的数据
                }
            }
            //for(int i = 0; i <= index; i++) 
             //   printf("%d ", num[i]);
            //printf("\n");
            if (prime != 0)
                printf("prime: %d\n", prime);
            close(fd[READ_FD]);
        }
    }
    // 筛选完成后最后一个进程结束
    exit(0);

}


