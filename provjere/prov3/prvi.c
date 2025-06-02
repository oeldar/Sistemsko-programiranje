#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

void mojHandler(int signum, siginfo_t* siginfo, void* ucontext) {
  printf("Moj handler.\n");
  printf("PID = %d\n", siginfo->si_pid);
  printf("SIGNUM = %d\n", siginfo->si_signo);
  printf("UID = %d\n", siginfo->si_uid);
  pause();
}

int main() {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = mojHandler;
  sigaddset(&sa.sa_mask, SIGINT);
  sigaction(SIGUSR1, &sa, NULL);


  pid_t pid = fork();
  if (pid != 0) {
    printf("Saljem SIGALRM childu.\n");
    kill(pid, SIGALRM);
  }
  

  printf("Sad saljem SIGUSR1.\n");
  raise(SIGUSR1);

  sigset_t myset;
  sigemptyset(&myset);
  sigaddset(&myset, SIGINT);
  sigprocmask(SIG_BLOCK, &myset, NULL);



  sigset_t pending;
  sigpending(&pending);

  if(sigismember(&pending, SIGINT)) {
    printf("Signal SIGINT je u pending stanju.\n");
  } else {
    printf("Interrupt nije u pending stanju.");
  }

  return 0;
}
