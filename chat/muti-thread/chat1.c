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

pthread_t thread_A_read, thread_A_write;

int fd_atob, fd_btoa;

void *thread_A_read_func(void *arg) {
    char message[256];
    while (1) {
        read(fd_btoa, message, sizeof(message));
        printf("[B]: %s", message);
    }
    return NULL;
}

void *thread_A_write_func(void *arg) {
    char message[256];
    while (1) {
        fgets(message, sizeof(message), stdin);
        write(fd_atob, message, strlen(message));
    }
    return NULL;
}

int main() {
    // 创建管道
    mkfifo("atob.txt", 0777);
    mkfifo("btoa.txt", 0777);

    // 打开管道文件描述符
    fd_atob = open(ATOB, O_WRONLY);
    fd_btoa = open(BTOA, O_RDONLY);
    if (fd_atob == -1 || fd_btoa == -1) {
        perror("open");
        exit(1);
    }

    // 创建线程
    pthread_create(&thread_A_read, NULL, thread_A_read_func, NULL);
    pthread_create(&thread_A_write, NULL, thread_A_write_func, NULL);

    // 等待线程结束
    pthread_join(thread_A_read, NULL);
    pthread_join(thread_A_write, NULL);

    // 关闭文件描述符
    close(fd_atob);
    close(fd_btoa);

    return 0;
}
