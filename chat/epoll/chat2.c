#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>

#define ATOB "atob.txt"
#define BTOA "btoa.txt"

int fd_atob, fd_btoa;

int main() {
    // 创建管道
    mkfifo("atob.txt", 0777);
    mkfifo("btoa.txt", 0777);

    // 打开管道文件描述符
    fd_atob = open(ATOB, O_RDONLY | O_NONBLOCK);
    fd_btoa = open(BTOA, O_WRONLY);
    if (fd_atob == -1 || fd_btoa == -1) {
        perror("open");
        exit(1);
    }

    // 设置标准输入非阻塞
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);

    // 创建epoll实例
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        exit(1);
    }

    // 添加stdin和fd_atob到epoll监视列表中
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event) == -1) {
        perror("epoll_ctl: stdin");
        exit(1);
    }
    event.data.fd = fd_atob;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd_atob, &event) == -1) {
        perror("epoll_ctl: fd_atob");
        exit(1);
    }

    while (1) {
        struct epoll_event events[2];
        int num_events = epoll_wait(epfd, events, 2, -1);
        if (num_events == -1) {
            perror("epoll_wait");
            exit(1);
        }

        for (int i = 0; i < num_events; ++i) {
            if (events[i].data.fd == STDIN_FILENO) {
                char message[256];
                fgets(message, sizeof(message), stdin);
                write(fd_btoa, message, strlen(message));
            } else if (events[i].data.fd == fd_atob) {
                char message[256];
                ssize_t bytes_read = read(fd_atob, message, sizeof(message));
                if (bytes_read > 0) {
                    printf("[A]: %s", message);
                }
            }
        }
    }

    // 关闭文件描述符
    close(fd_atob);
    close(fd_btoa);

    return 0;
}
