#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <poll.h>

#define ATOB "atob.txt"
#define BTOA "btoa.txt"

int fd_atob, fd_btoa;

int main() {
    // 创建管道
    mkfifo("atob.txt", 0777);
    mkfifo("btoa.txt", 0777);

    // 打开管道文件描述符
    fd_atob = open(ATOB, O_WRONLY);// 写操作不能阻塞，因为管道满了要阻塞
    fd_btoa = open(BTOA, O_RDONLY | O_NONBLOCK);// 读操作，当管道没有数据，读操作会一直等待，直到有数据
    if (fd_atob == -1 || fd_btoa == -1) {
        perror("open");
        exit(1);
    }

    // 设置标准输入非阻塞
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);

    // 设置pollfd结构体
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = fd_btoa;
    fds[1].events = POLLIN;

    while (1) {
        // 使用 poll() 进行多路监听
        int ready = poll(fds, 2, -1);
        if (ready == -1) {
            perror("poll");
            exit(1);
        }

        // 检查标准输入是否准备好
        if (fds[0].revents & POLLIN) {
            char message[256];
            fgets(message, sizeof(message), stdin);
            write(fd_atob, message, strlen(message));
        }

        // 检查管道是否有数据可读
        if (fds[1].revents & POLLIN) {
            char message[256];
            ssize_t bytes_read = read(fd_btoa, message, sizeof(message));
            if (bytes_read > 0) {
                printf("[B]: %s", message);
            }
        }
    }

    // 关闭文件描述符
    close(fd_atob);
    close(fd_btoa);

    return 0;
}
