#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fd = open("file1", O_CREAT | O_WRONLY, 0644);
  char anis[] = "baas\0  baas";
  int i = 0;
  while (i++ < 3){
    write(fd, anis, sizeof(anis));
  lseek(fd, 2, SEEK_END);
  }
  return 0;
}
