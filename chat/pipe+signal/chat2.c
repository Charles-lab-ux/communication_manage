#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>
void signal_handler(int signum){
    if(signum == SIGUSR2){
        char buf[1024];
        int fd_atob=open("atob",O_RDONLY);
        if(fd_atob==-1){
            perror("open");
            exit(1);
        }
        if(read(fd_atob,buf,1024)==-1){
            perror("read");
            exit(1);
        }
        printf("[A]: %s",buf);
        close(fd_atob);
    }
}
int main()
{
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
    printf("Let's talk!\n");
    char buf[1024];
    while(memset(buf,0,1024),read(0,buf,1024)!=0)
    {
        printf("[B]: %s",buf);
        write(fd_btoa,buf,strlen(buf));
        kill(Apid, SIGUSR1);
    }
    close(fd_btoa);
    close(fd_atob);
    printf("pipe exit!\n");
    return 0;
}
