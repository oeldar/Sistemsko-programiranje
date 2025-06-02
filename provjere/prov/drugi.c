#include <memory.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>

void fatal(char *message) {
  printf("%s\n", message);
  exit(1);
}

int main(int argc, char *argv[]) {
  int port = 7070;
  if (argc == 2) {
    port = atoi(argv[1]);
  }

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_port = htons(port);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    fatal("Bind error.");

  listen(sock, 1);
  printf("Server started on port: %d\n", port);

  struct sockaddr_in client;
  memset(&client, 0, sizeof(client));

  socklen_t client_len = sizeof(client);
  int cli = accept(sock, (struct sockaddr *)&client, &client_len);
  printf("Client connected.\n");

  fd_set readfds;
  struct timeval timeout;
  FD_ZERO(&readfds);
  FD_SET(cli, &readfds);
  timeout.tv_sec = 25;
  timeout.tv_usec = 0;

  int act = select(cli + 1, &readfds, NULL, NULL, &timeout);
  if (act == -1) {
    fatal("Select error.");
  } else if (act == 0) {
    printf("Timeout received.\n");
    close(cli);
    printf("Client disconnected.\n");
  } else {
    char buffer[1024];
    int n = recv(cli, buffer, sizeof(buffer) - 1, 0);
    if (n > 0)
      printf("%s\n", buffer);
    else
      printf("Connection closed.");
    close(cli);
    printf("Client disconnected.\n");
  }

  close(sock);

  return 0;
}
