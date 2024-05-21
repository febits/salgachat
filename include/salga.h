#ifndef SALGA_H
#define SALGA_H

#include <stdio.h>
#include <stdlib.h>

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

#endif
