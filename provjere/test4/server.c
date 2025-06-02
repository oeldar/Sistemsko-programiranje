#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_PATH1 "fifo1"
#define FIFO_PATH2 "fifo2"

int main() {
  int f1 = mkfifo(FIFO_PATH1, 0644);
  int f2 = mkfifo(FIFO_PATH2, 0644);

  int server_fifo = open(FIFO_PATH1, O_RDONLY);
  int client_fifo = open(FIFO_PATH2, O_WRONLY);

  char buffer[1024];
  int n;
  n = read(server_fifo, buffer, sizeof(buffer));
  buffer[n] = '\0';

  int broj = atoi(buffer);
  broj = broj * broj;

  char rez[1024];
  sprintf(rez, "%d", broj);

  write(client_fifo, rez, sizeof(rez));
  printf("%s\n", buffer);

  close(server_fifo);
  close(client_fifo);

  unlink(FIFO_PATH1);
  unlink(FIFO_PATH2);

  return 0;
}
