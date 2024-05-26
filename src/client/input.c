#include <stdbool.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "types.h"

#define DEL 0x7f
#define BS 0x08
#define NAK 0x15
#define ESC 0x1b

#define ARROW_UP 0x41
#define ARROW_DOWN 0x42
#define ARROW_RIGHT 0x43
#define ARROW_LEFT 0x44

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

    // break and continue points
    if (c == '\n') {
      break;
    } else if (c == ESC) {
      char ansi_codes[2] = {0};
      if (read(STDIN_FILENO, ansi_codes, 2) == 2 && ansi_codes[0] == '[') {
        switch (ansi_codes[1]) {
          case ARROW_UP:
          case ARROW_DOWN:
          case ARROW_LEFT:
          case ARROW_RIGHT:
            continue;
        }
      } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
        return false;
      }
    }

    // safe to interpret points
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
