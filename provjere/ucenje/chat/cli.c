#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char* argv[]){
  int port = atoi(argv[2]);
  char* serv_ip = argv[1];

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));

  server.sin_port = htons(port);
  server.sin_family = AF_INET;
  
  inet_pton(AF_INET, serv_ip, &server.sin_addr);
  connect(sock, (struct sockaddr*)&server, sizeof(server));

  pid_t pid = fork();

  if (pid == 0) {
    // neka dijete cita od servera
    char buff[1024];
    int n;
    n = read(sock, buff, sizeof(buff));
    buff[n] = '\0';
    while (n > 0) {
      printf("%s\n", buff);
      n = read(sock, buff, sizeof(buff));
      buff[n] = '\0';
    }

  } else {
    // neka roditelj pise serveru
    char buff[1024];
    int n;
    write(sock, buff, sizeof(buff));

  }

  return 0;
}
