#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main (int argc, char *argv[]) {
  int fd1 = open("foo.txt", O_CREAT | O_RDWR, 0644);
  int fd2 = open("bar.txt", O_CREAT | O_RDWR, 0644);


  write(fd1, "hej\n", 5);


  close(fd1);
  close(fd2);
  return 0;
}
