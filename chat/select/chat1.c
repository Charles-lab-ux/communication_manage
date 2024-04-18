#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>

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

    // 设置文件描述符集合
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(fd_btoa, &readfds);

    // 找出最大的文件描述符值
    int max_fd = (STDIN_FILENO > fd_btoa) ? STDIN_FILENO : fd_btoa;

    while (1) {
        // 使用 select() 进行多路监听
        int ready = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (ready == -1) {
            perror("select");
            exit(1);
        }

        // 检查标准输入是否准备好
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            char message[256];
            fgets(message, sizeof(message), stdin);
            write(fd_atob, message, strlen(message));
        }

        // 检查管道是否有数据可读
        if (FD_ISSET(fd_btoa, &readfds)) {
            char message[256];
            ssize_t bytes_read = read(fd_btoa, message, sizeof(message));
            if (bytes_read > 0) {
                printf("[B]: %s", message);
            }
        }

        // 重新设置文件描述符集合
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(fd_btoa, &readfds);
    }

    // 关闭文件描述符
    close(fd_atob);
    close(fd_btoa);

    return 0;
}
