#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
void fatal(char *message) {
  perror(message);
  exit(1);
}
void cls() { fprintf(stdout, "\033[2J\033[H"); }
void print_center(char *message) {
  struct winsize w;
  if (ioctl(0, TIOCGWINSZ, &w) < 0)
    fatal("failed getting term size!");
  fprintf(stdout, "\033[31m\033[%d;%ldH%s\033[0m", w.ws_row / 2,
          (w.ws_col - strlen(message)) / 2, message);
}
int main(int argc, char *argv[]) {
  cls();
  print_center("Pozdrav svima!\n");
  return 0;
}
