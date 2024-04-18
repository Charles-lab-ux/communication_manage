#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>
void signal_handler(int signum){
    if(signum == SIGUSR1){
        char buf[1024];
        memset(buf,0,1024);
        int fd_btoa=open("btoa",O_RDONLY);
        if(fd_btoa==-1){
            perror("open");
            exit(1);
        }
        if(read(fd_btoa,buf,1024)==-1){
            perror("read");
            exit(1);
        }
        printf("[B]: %s",buf);
        close(fd_btoa);
    }
}
int main(){
    // 创建命名管道
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
    printf("Let's talk!\n");
    char buf[1024];
    while(memset(buf,0,1024),read(0,buf,1024)!=0)
    {
        printf("[A]: %s",buf);
        write(fd_atob,buf,strlen(buf));
        kill(Bpid, SIGUSR2);
    }
    close(fd_atob);
    close(fd_btoa);
    printf("pipe exit!\n");
    return 0;
}
