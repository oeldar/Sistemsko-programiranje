#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#define PATH_MAX 512

char *fd_to_path(int fd) {
    char proc_path[64];  // putanja do /proc/self/fd/<fd>
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

    buf[len] = '\0';  // ručno završiti string
    return buf;       // caller mora free(buf)
}

int main (int argc, char *argv[]) {
  int fd1 = open("foo.txt", O_CREAT | O_RDWR, 0644);
  int fd2 = open("bar.txt", O_CREAT | O_RDWR, 0644);


  write(fd1, "hej\n", 5);
  dup2(fd1, fd2);
  write(fd1, "kako\n", 6);
  write(fd2, "sta\n", 5);

  std::cout << fd1 << " " << fd2 << std::endl;

  char path[1024];
snprintf(path, sizeof(path), "/proc/self/fd/%d", fd1);
char buf[1024];
ssize_t len = readlink(path, buf, sizeof(buf)-1);
if (len != -1) {
    buf[len] = '\0';
    printf("FD %d points to: %s\n", fd1, buf);
}


  close(fd1);
  close(fd2);
  return 0;
}
