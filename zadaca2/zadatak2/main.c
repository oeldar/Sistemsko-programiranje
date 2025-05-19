#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 9090
#define BUFFER_SIZE 1024
#define FIFO_PATH "/tmp/relay_fifo"

#define BLUE "\033[34m"
#define RESET "\033[0m"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void logger_process(int read_fd) {
    FILE *log_file = fopen("log.txt", "a");
    if (!log_file) error("Cannot open log.txt");

    char buffer[BUFFER_SIZE];
    while (1) {
        int n = read(read_fd, buffer, sizeof(buffer));
        if (n > 0) {
            buffer[n] = '\0';
            fprintf(log_file, "%s\n", buffer);
            fflush(log_file);
        }
    }

    fclose(log_file);
}

void receiver_process() {
    mkfifo(FIFO_PATH, 0666);
    char buffer[BUFFER_SIZE];
    int fifo_fd = open(FIFO_PATH, O_RDONLY);
    if (fifo_fd < 0) error("FIFO open error");

    while (1) {
        int n = read(fifo_fd, buffer, sizeof(buffer));
        if (n > 0) {
            buffer[n] = '\0';
            printf(BLUE "[Receiver] %s\n" RESET, buffer);
        }
    }

    close(fifo_fd);
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) error("pipe");

    pid_t logger_pid = fork();
    if (logger_pid == 0) {
        close(pipefd[1]); 
        logger_process(pipefd[0]);
        exit(0);
    }

    pid_t receiver_pid = fork();
    if (receiver_pid == 0) {
        receiver_process();
        exit(0);
    }

    int server_fd, client_fd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    char buffer[BUFFER_SIZE];

    unlink(FIFO_PATH);
    mkfifo(FIFO_PATH, 0666);
    int fifo_fd = open(FIFO_PATH, O_WRONLY);
    if (fifo_fd < 0) error("FIFO write open");

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) error("Socket failed");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Bind failed");

    listen(server_fd, 5);
    printf("Relay server listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");

        while (1) {
            int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (n <= 0) break;
            buffer[n] = '\0';

            write(pipefd[1], buffer, strlen(buffer));

            write(fifo_fd, buffer, strlen(buffer));
        }

        close(client_fd);
        printf("Client disconnected.\n");
    }

    close(server_fd);
    close(pipefd[1]);
    close(fifo_fd);
    unlink(FIFO_PATH);

    return 0;
}

