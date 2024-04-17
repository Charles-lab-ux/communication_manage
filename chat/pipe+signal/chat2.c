#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include<sys/stat.h>
#define ATOB "atob.txt"
#define BTOA "btoa.txt"
void signal_handler(int signum){
    if(signum == SIGUSR2){
        char message[256];
        int fd = open(ATOB, O_RDWR);
        read(fd, message, sizeof(message));
        printf("[A]: %s",message);
        close(fd);
    }
}
int main() {
    int fd_atob=open("atob",O_RDONLY);
    int fd_btoa=open("btoa",O_WRONLY);
    if(-1==fd_atob||-1==fd_btoa)
    {
        perror("open");
        exit(1);
    }
    int Apid;
    read(fd_atob,&Apid,sizeof(int));
    printf("Get A's pid: %d\n",Apid);
    int pid=getpid();
    write(fd_btoa,&pid,sizeof(int));
    printf("Send B's pid: %d\n",pid);

    signal(SIGUSR2,signal_handler);

    int fd;
    char message[256];

    // 打开文件
    fd = open(BTOA, O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // 读取文件中的消息并显示
    while (fgets(message, sizeof(message), stdin) != NULL) {
        write(fd, message, strlen(message));
        printf("[B]: %s",message);
        kill(Apid,SIGUSR1);
    }

    // 关闭文件
    close(fd);

    return 0;
}
