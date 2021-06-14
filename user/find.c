// shell中的find命令，作用是在目录树中寻找到所有对应名字的文件，应该是要用到递归了
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#define NULL (void*)0
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
    // 这里要把后面用转义0替换，示例代码挖坑了，使用空串进行了替换
    memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
    return buf;
}


void
find(char *path, char* file) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    // 用来装文件夹名字
    // 还是避免使用二维指针吧

    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
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
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            // 这里还是不能用这种，必须使用名字的比较
            if(de.inum == 0 || de.inum == 1)
                continue;   //忽略.和..两个文件夹
            if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))
                continue;
            // 这里将文件名丢到buf中， 十分巧妙的设计
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            // 这里开始出现与ls里面不一样的逻辑，要对文件夹里面的内容根据类型(文件夹还是文件进行分开的读取)
            switch (st.type)
            {
            case T_FILE:
                    //printf("%s\n", fmtname(buf));
                    //printf("%d\n", strcmp(file, fmtname(buf)));
                if(!strcmp(file, fmtname(buf)))
                    printf("%s\n", buf);
                break;
            case T_DIR:
                //printf("%s\n", buf);
                find(buf, file);
                break;
            }
        }
        break;
    }
}

int
main(int argc, char *argv[]) {
    if(argc < 2 || argc > 3){
        printf("paramer error...\n");
        exit(1);
    }
    if(argc == 2){
        find(".", argv[1]);
        exit(0);
    } else if (argc == 3) {
        find(argv[1], argv[2]);
        exit(0);
    }
    exit(0);
}