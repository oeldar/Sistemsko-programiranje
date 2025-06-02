#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

int ttyfd = STDIN_FILENO;
struct termios origTerm;

void resetTerm() {
  if (tcsetattr(ttyfd, TCSAFLUSH, &origTerm) == -1)
    perror("Error reseting terminal.");
}

void setTerm() {
  struct termios newTerm;
  newTerm = origTerm;
  newTerm.c_lflag &= ~(ICANON | ECHO);
  newTerm.c_cc[VMIN] = 0;
  newTerm.c_cc[VTIME] = 10;
  
  if (tcsetattr(ttyfd, TCSAFLUSH, &newTerm) == -1)
    perror("Error setting new terminal settings.");

}

void processTerm() {
    char c_in;
    int n;
    int red = 0; // Da li je aktivna crvena boja?

    while (1) {
        n = read(ttyfd, &c_in, sizeof(c_in));
        if (n < 0) {
            perror("Read error.");
            continue;
        } else if (n == 0) {
            write(STDOUT_FILENO, "W", 1);
            continue;
        }

        switch (c_in) {
            case 'q':
                return;

            case 'r':
                red = 1;
                // Uključi crvenu boju
                write(STDOUT_FILENO, "\033[31m", 5);
                break;

            case 'd':
                red = 0;
                // Vrati na normalnu boju
                write(STDOUT_FILENO, "\033[0m", 4);
                break;

            default:
                // Ispiši karakter u aktivnoj boji
                write(STDOUT_FILENO, &c_in, 1);
                break;
        }
    }
}


int main() {
  if (!isatty(ttyfd)) perror("Not a terminal.");
  if (tcgetattr(ttyfd, &origTerm) == -1) perror("Error getting original terminal settings.");
  if (atexit(resetTerm)) perror("Error reseting terminal.");

  setTerm();
  processTerm();

  return 0;
}
