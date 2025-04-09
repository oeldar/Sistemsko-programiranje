#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  int fd;
  fd = open("file1", O_RDWR | O_CREAT, 0644);

  if (fd == -1) {
    printf("%s", strerror(errno));
  }



  char buf[3];
  ssize_t bytesRead = 0;
  /* while ((bytesRead = read(STDIN_FILENO, buf, sizeof(buf))) > 0) { */
  /*   write(fd, buf, bytesRead); */
  /* } */

  /* while ( (bytesRead = read(fd, buf, sizeof(buf))) > 0) { */
  /*   write(STDOUT_FILENO, buf, bytesRead); */
  /* } */
  /**/



  close(fd);

  return 0;
}
