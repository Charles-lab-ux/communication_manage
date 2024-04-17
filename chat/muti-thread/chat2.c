#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>

#define ATOB "atob.txt"
#define BTOA "btoa.txt"

pthread_t thread_B_read, thread_B_write;

int fd_atob, fd_btoa;

void *thread_B_read_func(void *arg) {
    char message[256];
    while (1) {
        read(fd_atob, message, sizeof(message));
        printf("[A]: %s", message);
    }
    return NULL;
}

void *thread_B_write_func(void *arg) {
    char message[256];
    while (1) {
        fgets(message, sizeof(message), stdin);
        write(fd_btoa, message, strlen(message));
    }
    return NULL;
}

int main() {
    // 创建管道
    mkfifo("atob.txt", 0777);
    mkfifo("btoa.txt", 0777);

    // 打开管道文件描述符
    fd_atob = open(ATOB, O_RDONLY);
    fd_btoa = open(BTOA, O_WRONLY);
    if (fd_atob == -1 || fd_btoa == -1) {
        perror("open");
        exit(1);
    }

    // 创建线程
    pthread_create(&thread_B_read, NULL, thread_B_read_func, NULL);
    pthread_create(&thread_B_write, NULL, thread_B_write_func, NULL);

    // 等待线程结束
    pthread_join(thread_B_read, NULL);
    pthread_join(thread_B_write, NULL);

    // 关闭文件描述符
    close(fd_atob);
    close(fd_btoa);

    return 0;
}
