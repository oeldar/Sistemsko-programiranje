#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int ttyfd = STDIN_FILENO;
struct termios orig_termios;

void tty_atexit() {
  if (tcsetattr(ttyfd, TCSAFLUSH, &orig_termios) < 0)
    1;
}

void set_tty() {
  struct termios raw;
  raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 10;

  tcsetattr(ttyfd, TCSAFLUSH, &raw);
}

void process_term() {
  int n;
  char c_in, c_out;

  while(1) {
    read(ttyfd, &c_in, 1);
    switch(c_in) {
      case 'a':
        c_out = 'f';
        write(STDOUT_FILENO, &c_out, 1);

    }
  }
}

int main(void) {
  if (!isatty(ttyfd))
    return 0;

  if (tcgetattr(ttyfd, &orig_termios) < 0)
    return 0;

  if (atexit(tty_atexit) != 0)
    return 0;

  set_tty();
  process_term();

  return 0;
}
