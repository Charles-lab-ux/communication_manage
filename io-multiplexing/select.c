#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_FILES 5
#define MAX_BUF_SIZE 1024

void print_fd_set(fd_set *set, int max_fd) {
    printf("File descriptor set:\n");
    for (int fd = 0; fd < max_fd; fd++) {
        if (FD_ISSET(fd, set)) {
            printf("%d ", fd);
        }
    }
    printf("\n");
}

int main() {
    int fd_array[MAX_FILES]; // 存放文件描述符的数组
    char buffer[MAX_FILES][MAX_BUF_SIZE]; // 存放每个文件的内容
    int i, max_fd;

    // 打开多个日志文件，获取文件描述符
    for (i = 0; i < MAX_FILES; i++) {
        char filename[20];
        snprintf(filename, sizeof(filename), "logfile%d.txt", i);
        fd_array[i] = open(filename, O_RDONLY);
        printf("%d\n",fd_array[i]);
        if (fd_array[i] == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    // 获取最大文件描述符
    max_fd = fd_array[MAX_FILES - 1] + 1;
    printf("%d\n",max_fd);
    while (1) {
        fd_set read_fds;
        struct timeval timeout;

        // 设置超时时间
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // 初始化文件描述符集合
        FD_ZERO(&read_fds);
        for (i = 0; i < MAX_FILES; i++) {
            FD_SET(fd_array[i], &read_fds);
        }
        // 调用select
        int ret = select(max_fd, &read_fds, NULL, NULL, &timeout);
        // timeout起到设置超时时间的作用，select会等待一段时间来检查文件描述符的状态
        // 在这段时间内没有文件描述符准备好读取，select就会返回0
        if (ret == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        } else if (ret == 0) {
            printf("Timeout occurred!\n");
        } else {
            printf("Data is available now.\n");
            // 检查哪些文件有数据可读
            for (i = 0; i < MAX_FILES; i++) {
                // 检查fd_array[i]是否在read_fds集合中
                if (FD_ISSET(fd_array[i], &read_fds)) {
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
            sleep(3);
        }
    }

    // 关闭文件描述符
    for (i = 0; i < MAX_FILES; i++) {
        close(fd_array[i]);
    }

    return 0;
}
