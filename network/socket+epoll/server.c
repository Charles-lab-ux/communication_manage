#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define PORT 12345
#define MAX_EVENTS 10
#define MAX_BUFFER_SIZE 1024
#define FILE_PATH "./queue/received_file"

int file_count=0;
int main() {
    struct sockaddr_in server_addr, client_addr;
    int server_fd, client_fd, epoll_fd, event_count;
    struct epoll_event event, events[MAX_EVENTS];
    char buffer[MAX_BUFFER_SIZE];

    // 创建Socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//指定了服务器监听的地址
    server_addr.sin_port = htons(PORT);

    // 将Socket绑定到服务器
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听即将来临的连接，5是待处理连接请求的队列长度，超出队列长度的连接请求被拒绝
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // 创建epoll实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Epoll creation failed");
        exit(EXIT_FAILURE);
    }
    // 把服务器socket加到epoll
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("Epoll control failed");
        exit(EXIT_FAILURE);
    }
    printf("Server started. Listening on port %d...\n", PORT);

    while (1) {
        // 等待事件的发生
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (event_count == -1) {
            perror("Epoll wait failed");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < event_count; i++) {
            if (events[i].data.fd == server_fd) {
                // 检查有新的客户端连接请求
                socklen_t client_addr_len = sizeof(client_addr);
                client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                if (client_fd == -1) {
                    perror("Accept failed");
                    exit(EXIT_FAILURE);
                }

                printf("Connection established with client\n");

                // Add client socket to epoll instance
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    perror("Epoll control failed");
                    exit(EXIT_FAILURE);
                }
            } else {
                // 不是连接请求，表示是已连接的客户端发送了数据
                int bytes_received = recv(events[i].data.fd, buffer, MAX_BUFFER_SIZE, 0);
                if (bytes_received == -1) {
                    perror("Receive failed");
                    exit(EXIT_FAILURE);
                } else if (bytes_received == 0) {
                    // 客户端关闭
                    printf("Client disconnected\n");
                    close(events[i].data.fd);
                } else {
                    // 将接受的数据保存在文件里
                    char file_name[100];
                    snprintf(file_name, sizeof(file_name), "%s%d.txt", FILE_PATH, ++file_count);
                    FILE *file = fopen(file_name, "wb");
                    if (file == NULL) {
                        perror("File open failed");
                        exit(EXIT_FAILURE);
                    }

                    fwrite(buffer, sizeof(char), bytes_received, file);
                    printf("Received file saved as %s\n", FILE_PATH);

                    fclose(file);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
