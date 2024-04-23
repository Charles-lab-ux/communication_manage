#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define FILE_PATH "file_to_send.txt"
#define MAX_BUFFER_SIZE 1024

int main() {
    struct sockaddr_in server_addr;
    int client_fd;
    FILE *file;
    char buffer[MAX_BUFFER_SIZE];

    // 打开传输的文件 可读
    file = fopen(FILE_PATH, "r");
    if (file == NULL) {
        perror("File open failed");
        exit(EXIT_FAILURE);
    }

    // 创建Socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器IP地址+端口号
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // 连接到服务器
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    // 读文件并传输到服务器
    while (fgets(buffer, MAX_BUFFER_SIZE, file) != NULL) {
        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
    }

    printf("File sent successfully\n");

    fclose(file);
    close(client_fd);

    return 0;
}
