#include <arpa/inet.h>
#include <memory.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

void fatal(char *message) {
  printf("%s", message);
  exit(1);
}

void send404(int csock) {
  char response[] = "Page not found.\n";
  send(csock, response, sizeof(response), 0);
}

void sendFile(int csock, char* fullPath) {
  int fd = open(fullPath, O_RDONLY);

  int bytesRead;
  char buffer[BUFFER_SIZE];
  while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
    send(csock, buffer, bytesRead, 0);
  }
}

void sendDirectory(int csock, char* fullPath) {
  DIR* dir = opendir(fullPath);

  if (!dir) send404(csock);

  char indexPath[1024];
  snprintf(indexPath, sizeof(indexPath), "%s/index.html", fullPath);

  struct stat st;
  if (stat(indexPath, &st) == 0) {
    sendFile(csock, indexPath);
  } else {
    struct dirent* entry;
    while((entry = readdir(dir)) != NULL) {
      printf("%s\n", entry->d_name);
    }

  }


 
}

void handleRequest(int csock) {
  char buffer[BUFFER_SIZE];
  int n = read(csock, buffer, sizeof(buffer));
  if (n < 0)
    fatal("Error with reading client socket.");

  buffer[n] = '\0';

  char method[8], path[1024];
  sscanf(buffer, "%s %s", method, path);

  if (strcmp(method, "GET") != 0) {
    send404(csock);
    return;
  }

  char fullPath[1024] = ".";
  strcat(fullPath, path);

  struct stat st;
  stat(fullPath, &st);

  printf("Trazeni resurs: %s", fullPath);
  if (S_ISREG(st.st_mode)) {
    printf("Saljem fajl.\n");
    sendFile(csock, fullPath);
  } else if (S_ISDIR(st.st_mode)){
    printf("Saljem direktorij.\n");
    sendDirectory(csock, fullPath);
  }
}

in_addr_t getClientIP(struct sockaddr_in clientInfo) {
  return clientInfo.sin_addr.s_addr;
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    fatal("Port required.\n");

  int port;
  if (!(port = atoi(argv[1])))
    fatal("Port not valid.\n");

  int ssock;
  if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    fatal("Failed to make socket.\n");

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));

  server.sin_port = htons(port);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(ssock, (struct sockaddr *)&server, sizeof(server)) == -1)
    fatal("Bind fatal.\n");

  listen(ssock, 10);
  printf("Server started on port: %d\n", port);

  while (1) {
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t client_len = sizeof(client);

    int csock;
    if ((csock = accept(ssock, (struct sockaddr *)&client, &client_len)) == -1)
      fatal("Error while establishing connection.\n");




    printf("Client %s connected.\n", inet_ntoa(client.sin_addr));



    pid_t pid = fork();
    if (pid) {
      // parent:
      close(csock);
    } else {
      // child:
      close(ssock);
      handleRequest(csock);
      close(csock);
      printf("Client %s disconnected.\n", inet_ntoa(client.sin_addr));
      exit(1);
    }
  }

  close(ssock);

  return 0;
}
