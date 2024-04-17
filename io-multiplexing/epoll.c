#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_FILES 5
#define MAX_EVENTS 10
#define MAX_BUF_SIZE 1024

int main() {
    int fd_array[MAX_FILES]; // 存放文件描述符的数组
    char buffer[MAX_FILES][MAX_BUF_SIZE]; // 存放每个文件的内容
    int i;

    // 打开多个日志文件，获取文件描述符
    for (i = 0; i < MAX_FILES; i++) {
        char filename[20];
        snprintf(filename, sizeof(filename), "logfile%d.txt", i);
        fd_array[i] = open(filename, O_RDONLY);
        if (fd_array[i] == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    // 创建 epoll 实例
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // 添加文件描述符到 epoll 实例中
    for (i = 0; i < MAX_FILES; i++) {
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = fd_array[i];
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_array[i], &event) == -1) {
            perror("epoll_ctl: add");
            exit(EXIT_FAILURE);
        }
    }

    while (1) {
        struct epoll_event events[MAX_EVENTS];
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        
        if (num_events == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        printf("Data is available now.\n");
        // 检查哪些文件有数据可读

        for (i = 0; i < num_events; i++) {
            int fd = events[i].data.fd;
            ssize_t bytes_read = read(fd, buffer[i], MAX_BUF_SIZE);
            if (bytes_read == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            } else if (bytes_read == 0) {
                printf("EOF reached for file %d.\n", i);
            } else {
                printf("Data read from file %d: %s\n", i, buffer[i]);
            }
        }
        sleep(3);
    }

    // 关闭 epoll 实例
    close(epoll_fd);

    // 关闭文件描述符
    for (i = 0; i < MAX_FILES; i++) {
        close(fd_array[i]);
    }

    return 0;
}
