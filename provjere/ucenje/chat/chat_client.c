// chat_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

void fatal(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 3)
        fatal("Usage: ./chat_client <server_ip> <port>");

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        fatal("Socket creation failed");

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server.sin_addr) <= 0)
        fatal("Invalid address");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        fatal("Connection failed");

    printf("Connected to server!\n");

    pid_t pid = fork();
    if (pid == 0) {
        // Dijete: šalje poruke ka serveru
        char msg[BUFF_SIZE];
        while (fgets(msg, BUFF_SIZE, stdin) != NULL) {
            send(sock, msg, strlen(msg), 0);
        }
    } else {
        // Roditelj: prima poruke od servera
        char buffer[BUFF_SIZE];
        int n;
        while ((n = recv(sock, buffer, BUFF_SIZE - 1, 0)) > 0) {
            buffer[n] = '\0';
            printf("Server: %s", buffer);
        }
        printf("Server zatvorio konekciju.\n");
    }

    close(sock);
    return 0;
}

