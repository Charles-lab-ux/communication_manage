#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define MAX_FILES 5
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

    while (1) {
        struct pollfd fds[MAX_FILES];
        int timeout = 5000; // 5 秒超时

        // 设置 pollfd 结构体数组
        for (i = 0; i < MAX_FILES; i++) {
            fds[i].fd = fd_array[i];
            fds[i].events = POLLIN;
            fds[i].revents = 0;
        }

        // 调用 poll
        int ret = poll(fds, MAX_FILES, timeout);
        if (ret == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        } else if (ret == 0) {
            printf("Timeout occurred!\n");
        } else {
            printf("Data is available now.\n");
            // 检查哪些文件有数据可读
            for (i = 0; i < MAX_FILES; i++) {
                if (fds[i].revents & POLLIN) {
                    ssize_t bytes_read = read(fd_array[i], buffer[i], MAX_BUF_SIZE);
                    if (bytes_read == -1) {
                        perror("read");
                        exit(EXIT_FAILURE);
                    } else if (bytes_read == 0) {
                        printf("EOF reached for file %d.\n", i);
                    } else {
                        printf("Data read from file %d: %s\n", i, buffer[i]);
                    }
                }
            }
        }
        sleep(3);
    }

    // 关闭文件描述符
    for (i = 0; i < MAX_FILES; i++) {
        close(fd_array[i]);
    }

    return 0;
}
