#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define FIFO_PATH1 "fifo1"
#define FIFO_PATH2 "fifo2"

int main() {
  int server_fifo = open(FIFO_PATH1, O_WRONLY);
  int client_fifo = open(FIFO_PATH2, O_RDONLY);


  char number[1024];
  scanf("%s", number);
  write(server_fifo, number, sizeof(number));

  char buffer[1024];
  int n;
  n = read(client_fifo, buffer, sizeof(buffer));
  buffer[n] = '\0';

  printf("%s\n", buffer);

  close(server_fifo);
  close(client_fifo);
  return 0;
}
