#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_PATH1 "fifo1"
#define FIFO_PATH2 "fifo2"

int main() {
  int f1 = mkfifo(FIFO_PATH1, 0644);
  int f2 = mkfifo(FIFO_PATH2, 0644);

  pid_t pid = fork();
  if (pid == 0){
    int fd1 = open(FIFO_PATH1, O_RDONLY);
    int fd2 = open(FIFO_PATH2, O_WRONLY);

    char* writeBuffer = "Hej kako si?\n";
    write(fd2, writeBuffer, strlen(writeBuffer));

    char readBuffer[1024];
    int n;
    n = read(fd1, readBuffer, sizeof(readBuffer));
    readBuffer[n] = '\0';
    printf("Od drugog: %s\n", readBuffer);

  close(fd1);
  close(fd2);
  } else {
    int fd1 = open(FIFO_PATH1, O_WRONLY);
    int fd2 = open(FIFO_PATH2, O_RDONLY);

    char* writeBuffer = "Dobro sam, a ti?\n";
    write(fd1, writeBuffer, strlen(writeBuffer));

    char readBuffer[1024];
    int n;
    n = read(fd2, readBuffer, sizeof(readBuffer));
    readBuffer[n] = '\0';
    printf("Od prvog: %s\n", readBuffer);


  close(fd1);
  close(fd2);
  }

  return 0;
}
