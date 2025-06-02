#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  pid_t pid = fork();

  if (pid == 0) {
    printf("Child process. PID: %d\n", getpid());
  } else if (pid > 0) {
    printf("Parent process. PID: %d, Child Pid: %d\n", getpid(), pid);
  } else {
    perror("fork failed");
  }
  sleep(30);
  return 0;
}
