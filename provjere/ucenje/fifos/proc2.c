#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define FIFO_PATH "fifo"

int main() {
  int fd = open(FIFO_PATH, O_RDONLY);
  char buff[1024];
  int n;

  while (1) {
    n = read(fd, buff, sizeof(buff));
    write(STDOUT_FILENO, buff, n);
  }
  return 0;
}
