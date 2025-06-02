#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void mojHandler(int signum, siginfo_t *siginfo, void *ucontext) {
  /* static int num = 0; */
  /* num++; */
  /* printf("Sad cu odspavat.\n"); */
  /* sleep(3); */
  printf("Pozdrav iz mog handlera.\n");
  printf("Signal je poslao proces id = %d\n", siginfo->si_pid);
  /* if (num < 3)  */
  /* raise(SIGALRM); */
  pause();
}

int main() {
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = mojHandler;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGTSTP);
  sigaddset(&sa.sa_mask, SIGINT);

  sigaction(SIGALRM, &sa, NULL);
  raise(SIGALRM);

  pid_t parent = getpid();
  pid_t pid = fork();
  if (pid == 0)
    kill(parent, SIGALRM);
  else
    wait(NULL);


  sigset_t myset;
  sigemptyset(&myset);
  sigaddset(&myset, SIGINT);
  sigprocmask(SIG_BLOCK, &myset, NULL);
  /* alarm(3); */
  /* pause(); */
  return 0;
}
