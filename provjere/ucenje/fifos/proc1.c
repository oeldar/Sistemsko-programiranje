#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define FIFO_PATH "fifo"

int main() {
  int fd = open(FIFO_PATH, O_CREAT | O_WRONLY, 0644);

  char buff[1024];
  int n;
  while(1) {
    n = read(STDIN_FILENO, buff, sizeof(buff));
    if (n > 0)
      write(fd, buff, n);
  }

  return 0;
}
