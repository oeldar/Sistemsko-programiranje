#include <memory.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFF_SIZE 1024

void fatal(char *message) {
  printf("%s\n", message);
  exit(1);
}

void handleRequest(int cliSock) {
  printf("Handlam zahtijev klijenta.\n");

  char buffer[BUFF_SIZE];
  int recieved;


  char buffer_send[BUFF_SIZE];
  int sent;

  while (1) {
    recieved = read(cliSock, buffer, sizeof(buffer));
    if (recieved == 0) {
      printf("Client disconnected.\n");
      return;
    }
    buffer[recieved] = '\0';

    printf("%s", buffer);

    sent = read(STDIN_FILENO, buffer_send, sizeof(buffer_send));
    buffer_send[sent] = '\0';
    write(cliSock, buffer_send, sizeof(buffer_send));




  }



  return;
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    fatal("Port required.");

  int port = atoi(argv[1]);

  int servSock = socket(AF_INET, SOCK_STREAM, 0);
  if (servSock == -1)
    fatal("Server socket initialization problem.");

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));

  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  server.sin_family = AF_INET;

  int b = bind(servSock, (struct sockaddr *)&server, sizeof(server));
  if (b == -1)
    fatal("Bind error.");

  listen(servSock, 10);
  printf("Server started on port: %d\n", port);

  while (1) {
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t client_len = sizeof(client);
    int cliSock = accept(servSock, (struct sockaddr *)&client, &client_len);
    printf("Client connected.\n");

    pid_t pid = fork();
    if (pid == 0) {
      // child
      close(servSock);
      handleRequest(cliSock);
      close(cliSock);
      printf("Client disconnected.\n");
      exit(1);

    } else {
      // parent
      close(cliSock);
    }
  }

  return 0;
}
