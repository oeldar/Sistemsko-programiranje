#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *fd_to_path(int fd) {
  char proc_path[64]; // putanja do /proc/self/fd/<fd>
  snprintf(proc_path, sizeof(proc_path), "/proc/self/fd/%d", fd);

  char *buf = malloc(PATH_MAX);
  if (!buf) {
    perror("malloc");
    return NULL;
  }

  ssize_t len = readlink(proc_path, buf, PATH_MAX - 1);
  if (len == -1) {
    perror("readlink");
    free(buf);
    return NULL;
  }

  buf[len] = '\0'; // ručno završiti string
  return buf;      // caller mora free(buf)
}

int main(void) {
  int fd1 = open("fajl1.txt", O_RDWR | O_CREAT, 0644);
  int fd2 = open("fajl2.txt", O_RDWR | O_CREAT, 0644);

  printf("STDIN_FILENO = %d -> %s\n", STDIN_FILENO, fd_to_path(STDIN_FILENO));
  printf("STDOUT_FILENO = %d -> %s\n", STDOUT_FILENO,
         fd_to_path(STDOUT_FILENO));
  printf("STDERR_FILENO = %d -> %s\n", STDERR_FILENO,
         fd_to_path(STDERR_FILENO));
  printf("fd1 = %d -> %s\n", fd1, fd_to_path(fd1));
  printf("fd2 = %d -> %s\n", fd2, fd_to_path(fd2));

  printf("---------------------------\n");
  dup2(fd1, fd2);
  printf("STDIN_FILENO = %d -> %s\n", STDIN_FILENO, fd_to_path(STDIN_FILENO));
  printf("STDOUT_FILENO = %d -> %s\n", STDOUT_FILENO,
         fd_to_path(STDOUT_FILENO));
  printf("STDERR_FILENO = %d -> %s\n", STDERR_FILENO,
         fd_to_path(STDERR_FILENO));
  printf("fd1 = %d -> %s\n", fd1, fd_to_path(fd1));
  printf("fd2 = %d -> %s\n", fd2, fd_to_path(fd2));


  printf("Od ovog trenutka ce izlaz biti u fajlu fajl1.txt\n");
  dup2(fd1, STDOUT_FILENO);
  fflush(stdout);
  printf("---------------------------\n");
  printf("STDIN_FILENO = %d -> %s\n", STDIN_FILENO, fd_to_path(STDIN_FILENO));
  printf("STDOUT_FILENO = %d -> %s\n", STDOUT_FILENO,
         fd_to_path(STDOUT_FILENO));
  printf("STDERR_FILENO = %d -> %s\n", STDERR_FILENO,
         fd_to_path(STDERR_FILENO));
  printf("fd1 = %d -> %s\n", fd1, fd_to_path(fd1));
  printf("fd2 = %d -> %s\n", fd2, fd_to_path(fd2));

  dup2(STDIN_FILENO, STDOUT_FILENO);
  printf("Sad sam se vratio na terminal.\n");



  close(fd1);
  close(fd2);
  return EXIT_SUCCESS;
}
