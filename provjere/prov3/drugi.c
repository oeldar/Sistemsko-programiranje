#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int signum) {
  printf("Primio sam signal SIGINT.\n");
}

int main() {
  signal(SIGINT, handler);
  printf("Pritisni Ctrl+C.\n");
  sleep(4);
  printf("Kraj programa.\n");
  return 0;
}
