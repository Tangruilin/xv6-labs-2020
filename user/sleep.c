#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 这部分就直接复制一下，这里是对系统调用的考察，难度不是很大
int     //维持这里面的代码风格
main(int argc, char const *argv[])
{
    if(argc != 2) {
        fprintf(2, "parameters error...\n");
        exit(1);
    }
    sleep(atoi(argv[1]));
    exit(0);
}
