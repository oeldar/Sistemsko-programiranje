#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>


int ttyfd = STDIN_FILENO;
struct termios orig_termios;

void fatal(char* message) {
  printf("%s", message);
  exit(1);
}

void set_tty(){
  struct termios raw;
  raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 10;

  if (tcsetattr(ttyfd, TCSAFLUSH, &raw) < 0) fatal("Failed to set terminal to raw mode.");
}

void process_term() {
  int n;
  char c_in, c_out;

  while(1) {
    n = read(ttyfd, &c_in, 1);
    if (n < 0) fatal("");
    else if (n == 0) {
      c_out = 'T';
      write(STDOUT_FILENO, &c_out, 1);

    } else {
      switch(c_in) {
        case 'q':
          return;
          break;
        case 'a':
          write(STDOUT_FILENO, "anis pusi moju kurcinu.", 24);
      }

    }
  }


}

void reset_tty() {
  tcsetattr(ttyfd, TCSAFLUSH, &orig_termios);
}

void tty_atexit() {
  reset_tty();
  exit(0);
}

int main(){
  if (!isatty(ttyfd)) fatal("Not a terminal.");

  if (tcgetattr(ttyfd, &orig_termios) < 0) fatal("Failed to get original settings.");

  if (atexit(tty_atexit) != 0) fatal("Atexit error.");

  set_tty();
  process_term();
  return 0;
}
