#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main() {
  int fd = open("anis.txt", O_RDWR | O_CREAT, 0644);
  int fd2 = dup(fd);

  write(fd2, "anis\n", 5);
  close(fd);
  close(fd2);
  return 0;
}
