#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

struct termios orig_termios;
int ttyfd = STDIN_FILENO;

int tty_reset() {
  if (tcsetattr(ttyfd, TCSAFLUSH, &orig_termios) < 0)
    return -1;
  return 0;
}

void tty_atexit() {
  tty_reset();
}

void fatal(char* message) {
  perror(message);
  exit(1);
}

void set_tty() {
  struct termios raw;
  raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 10;
  if (tcsetattr(ttyfd, TCSAFLUSH, &raw) < 0) {
    fatal("Can't set raw mode.");
  }
}

int process_term() {
  int n;
  char c_in, c_out, up[]="\033[A";

  while(1) {
    n = read(ttyfd, &c_in, 1);
    if (n < 0) fatal("read error");
    if (n == 0) {
      c_out = 'T';
      write(STDOUT_FILENO, &c_out, 1);
    }
    else {
      switch (c_in) {
        case 'q':
          return 0;
        case 'z':
          write(STDOUT_FILENO, "Z", 1);
          break;
        case 'u':
          write(STDOUT_FILENO, up, 3);
          break;
        default:
          c_out = '*';
          write(STDOUT_FILENO, &c_out, 1);
      }
    }
  }
}

int main() {
  if (!isatty(ttyfd)) fatal("not on a tty.");

  if (tcgetattr(ttyfd, &orig_termios) < 0) fatal("can't get tty settings.");

  if (atexit(tty_atexit) != 0) fatal("atexit.");

  set_tty();
  process_term();



  return 0;
}
