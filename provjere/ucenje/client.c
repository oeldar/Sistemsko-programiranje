#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

void fatal(char *message) {
    printf("%s\n", message);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 3)
        fatal("Usage: ./client <server_ip> <port>");

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        fatal("Socket error");

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server.sin_addr) <= 0)
        fatal("Invalid address or address not supported");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        fatal("Connection failed");

    printf("Connected to server at %s:%d\n", server_ip, port);

    char buffer[BUFF_SIZE];
    while (fgets(buffer, BUFF_SIZE, stdin) != NULL) {
        send(sock, buffer, strlen(buffer), 0);
    }

    char buff[BUFF_SIZE];
    int n;
    while(1) {
      n = read(sock, buff, sizeof(buff));
      buff[n] = '\0';
      printf("%s", buff);
    }

    close(sock);
    return 0;
}

