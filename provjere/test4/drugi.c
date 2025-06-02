#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  int fds1[2];
  int fds2[2];

  int pipe1 = pipe(fds1);
  int pipe2 = pipe(fds2);

  pid_t pid = fork();

  if (pid == 0) {
    close(fds1[0]);
    close(fds2[1]);

    char *writeBuff = "Hej kako si? (p1)\n";
    write(fds1[1], writeBuff, strlen(writeBuff));

    char buffer[1024];
    int n;
    n = read(fds2[0], buffer, sizeof(buffer));
    buffer[n] = '\0';
    printf("Od drugog (p1): %s", buffer);

    exit(1);

  } else {
    close(fds1[1]);
    close(fds2[0]);

    char *writeBuff = "Dobro sam. (p2)\n";
    write(fds2[1], writeBuff, strlen(writeBuff));

    char buffer[1024];
    int n;
    n = read(fds1[0], buffer, sizeof(buffer));
    buffer[n] = '\0';

    printf("Od prvog (p2): %s\n", buffer);
    wait(NULL);
  }

  return 0;
}
