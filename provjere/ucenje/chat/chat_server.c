// chat_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

void fatal(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2)
        fatal("Usage: ./chat_server <port>");

    int port = atoi(argv[1]);
    int servSock = socket(AF_INET, SOCK_STREAM, 0);
    if (servSock < 0)
        fatal("Socket creation failed");

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(servSock, (struct sockaddr *)&server, sizeof(server)) < 0)
        fatal("Bind failed");

    listen(servSock, 1);
    printf("Waiting for connection on port %d...\n", port);

    struct sockaddr_in client;
    socklen_t clientLen = sizeof(client);
    int cliSock = accept(servSock, (struct sockaddr *)&client, &clientLen);
    if (cliSock < 0)
        fatal("Accept failed");

    printf("Client connected!\n");

    pid_t pid = fork();
    if (pid == 0) {
        // Dijete: šalje poruke ka klijentu
        char msg[BUFF_SIZE];
        while (fgets(msg, BUFF_SIZE, stdin) != NULL) {
            send(cliSock, msg, strlen(msg), 0);
        }
    } else {
        // Roditelj: prima poruke od klijenta
        char buffer[BUFF_SIZE];
        int n;
        while ((n = recv(cliSock, buffer, BUFF_SIZE - 1, 0)) > 0) {
            buffer[n] = '\0';
            printf("Klijent: %s", buffer);
        }
        printf("Klijent se diskonektovao.\n");
    }

    close(cliSock);
    close(servSock);
    return 0;
}

