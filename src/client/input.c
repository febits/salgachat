#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>

#include "types.h"

#define DEL 0x7f
#define BS 0x08
#define NAK 0x15

bool noncanon_input(char *dst, u64 max) {
  struct termios old = {0};
  struct termios new = {0};

  tcgetattr(STDIN_FILENO, &old);
  new = old;
  new.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new);

  u64 i = 0;

  fflush(stdout);
  fflush(stdin);

  while (true) {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) {
      tcsetattr(STDIN_FILENO, TCSANOW, &old);
      return false;
    }

    if (c == '\n') {
      break;
    }

    if ((c == DEL || c == '\b' || c == BS) && i > 0) {
      dst[--i] = '\0';
      printf("\b \b");
    } else if (c == NAK) {
      while (i > 0) {
        printf("\b \b");
        i--;
      }
      dst[0] = '\0';
    } else if (i < max && c != DEL && c != '\b' && c != BS && c != NAK) {
      dst[i++] = c;
      dst[i] = '\0';
      putchar(c);
    }

    fflush(stdout);
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &old);
  return true;
}
