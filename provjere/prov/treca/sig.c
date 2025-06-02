#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void mojHandler(int signum, siginfo_t *siginfo, void *ucontext) {
  printf("Sad cu odspavat\n");
  sigset_t blockedSignals;
  
  sleep(3);
  printf("Moj handler.\n");
}

int main() {
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = mojHandler;

  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGINT);
  sigaction(SIGALRM, &sa, NULL);

  sigset_t myset;
  sigemptyset(&myset);
  sigaddset(&myset, SIGINT);
  sigprocmask(SIG_BLOCK, &myset, NULL);
  raise(SIGALRM);

  sigset_t pendingSignals;
  sigpending(&pendingSignals);

  if (sigismember(&pendingSignals, SIGINT))
    printf("Signal SIGINT is pending.\n");
  return 0;
}
