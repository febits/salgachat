#ifndef SALGA_H
#define SALGA_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

#define SALGABANNER                                                            \
  " __                              \n"                                        \
  "(_   _. |  _   _.  _ |_   _. _|_ \n"                                        \
  "__) (_| | (_| (_| (_ | | (_|  |_ \n"                                        \
  "           _|                    \n"                                        \
  "\n"

#define error(fmt, ...)                                                        \
  fprintf(stderr, fmt, ##__VA_ARGS__);                                         \
  exit(EXIT_SUCCESS);

#define usage(fmt, ...) error(fmt, ##__VA_ARGS__)

#define USERSIZE 16
#define MSGSIZE 1024

#define SF_CONNECTING (1 << 0)
#define SF_UNAVAILABLE (1 << 1)

#define SC_LISTCMD (1 << 0)
#define SC_EXITCMD (1 << 1)

typedef struct {
  u8 flags;
  u8 commands;
  char user[USERSIZE + 1];
  char msg[MSGSIZE + 1];
} __attribute__((packed)) salgachat_pkt;

#endif
