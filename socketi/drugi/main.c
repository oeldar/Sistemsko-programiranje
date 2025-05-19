#include <memory.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFF_SIZE 1024

void relayProcess() {
  printf("U relay sam procesu.\n");

  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Omogućavanje ponovne upotrebe adrese (port-a)
  int opt = 1;
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    close(sock_fd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv;
  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(8080);
  serv.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock_fd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
    perror("bind");
    close(sock_fd);
    exit(EXIT_FAILURE);
  }

  if (listen(sock_fd, 10) < 0) {
    perror("listen");
    close(sock_fd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in cli;
  memset(&cli, 0, sizeof(cli));
  socklen_t cli_len = sizeof(cli);

  while (1) {
    int cli_fd = accept(sock_fd, (struct sockaddr *)&cli, &cli_len);
    if (cli_fd < 0) {
      perror("accept");
      close(sock_fd);
      exit(EXIT_FAILURE);
    }

    char buffer[BUFF_SIZE];

    while (1) {
      int bytesRead =
          read(cli_fd, buffer, BUFF_SIZE - 1); // ostavi mjesto za '\0'

      if (bytesRead <= 0) {
        if (bytesRead == 0) {
          printf("Konekcija zatvorena od strane klijenta.\n");
        } else {
          perror("read");
        }
        break;
      }

      buffer[bytesRead] = '\0'; // sigurno terminiranje stringa
      printf("%s", buffer);
    }
  

  close(cli_fd);
  }
  close(sock_fd);
}

int main() {
  relayProcess();
  return 0;
}
