#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include<signal.h>
#include<sys/stat.h>
#define ATOB "atob.txt"
#define BTOA "btoa.txt"
void signal_handler(int signum){
    if(signum == SIGUSR1){
        char message[256];
        int fd = open(BTOA, O_RDWR);
        read(fd, message, sizeof(message));
        printf("[B]: %s",message);   
        close(fd);
    }
}
int main() {
    // 管道发送进程号
    mkfifo("atob",0777);
    mkfifo("btoa",0777);
    int fd_atob=open("atob",O_WRONLY);
    int fd_btoa=open("btoa",O_RDONLY);
    if(-1==fd_atob||-1==fd_btoa)
    {
        perror("open");
        exit(1);
    }
        printf("Send A's pid: %d\n", getpid());
    int pid=getpid();
    write(fd_atob,&pid,sizeof(int));

    int Bpid;
    read(fd_btoa,&Bpid,sizeof(int));
    printf("Get B's pid: %d\n", Bpid);

    signal(SIGUSR1,signal_handler);
    int fd;
    char message[256];

    // 打开文件
    fd = open(ATOB, O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // 从标准输入读取消息，写入文件
    while (fgets(message, sizeof(message), stdin) != NULL) {
        write(fd, message, strlen(message));
        printf("[A]: %s",message);
        kill(Bpid,SIGUSR2);
    }

    // 关闭文件
    close(fd);

    return 0;
}
