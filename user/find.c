// shell中的find命令，作用是在目录树中寻找到所有对应名字的文件，应该是要用到递归了
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 文件名提取
char*
fmtname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;

    for(p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;

    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}


void
find(char *path, char* file) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    // 用来装文件夹名字
    char **dirs;

    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return
    }
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    // 对于文件是两种不同的类型的时候，进行解析
    switch (st.type)
    {
    case T_FILE:
        if (strcmp(file, fmtname(path)) == 0)
            printf("%s", path);
        break;
    // 如果是一个文件夹
    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        // 读取文件夹里面每一个元素，进行解析
        break;
    }
    
}