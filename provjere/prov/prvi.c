#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fds1[2];
  int fds2[2];

  int pipe1 = pipe(fds1);
  int pipe2 = pipe(fds2);

  pid_t pid = fork();

  if (pid == 0) {
    // child:
    close(fds1[0]);
    close(fds2[1]);

    // 0 -> read
    // 1 -> write

    char *writeBuffer = "Hej kako si?\n";
    write(fds1[1], writeBuffer, strlen(writeBuffer));

    char readBuffer[1024];
    int n;
    n = read(fds2[0], readBuffer, sizeof(readBuffer));
    readBuffer[n] = '\0';
    printf("Od drugog: %s", readBuffer);
  } else {
    // parent
    close(fds1[1]);
    close(fds2[0]);

    char *writeBuffer = "ja sam dobro?\n";
    write(fds2[1], writeBuffer, strlen(writeBuffer));

    char readBuffer[1024];
    int n;
    n = read(fds1[0], readBuffer, sizeof(readBuffer));
    readBuffer[n] = '\0';
    printf("Od prvog: %s", readBuffer);
  }


  return 0;
}
