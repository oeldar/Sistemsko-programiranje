#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

int ttyfd = STDIN_FILENO;
struct termios origTerm;

void fatal(const char* message) {
  printf("%s", message);
  exit(1);
}


void resetTerm() {
  if (tcsetattr(ttyfd, TCSAFLUSH, &origTerm) == -1) fatal("Error reseting terminal settings.");
}

void set_tty() {
  struct termios newTerm;
  newTerm = origTerm;

  newTerm.c_lflag &= ~(ICANON | ECHO | ISIG);
  newTerm.c_cc[VMIN] = 1;
  newTerm.c_cc[VTIME] = 10;

  if (tcsetattr(ttyfd, TCSAFLUSH, &newTerm) == -1) fatal("Error setting new settings.");
  return;
}

void processTerm() {
  char c_in;
  char* message = "unos";
  int n;
  while(1) {
    n = read(ttyfd, &c_in, 1);
    if (n == 1) write(STDOUT_FILENO, "-", 2);
    switch(c_in) {
      case 'w':
        write(STDOUT_FILENO, message, 5);
        break;
      case 'q':
        exit(1);
        break;
      default:
        break;
    }
  }
}

int main() {
  if (!isatty(ttyfd)) fatal("Not a terminal.");
  if (tcgetattr(ttyfd, &origTerm) == -1) fatal("Error getting terminal settings.");
  if (atexit(resetTerm) == -1) fatal("Error reseting terminal settings.");

  set_tty();
  processTerm();
  return 0;
}
