#ifndef SALGA_H
#define SALGA_H

#include <stdio.h>
#include <stdlib.h>

#define error(fmt, ...)                                                        \
  fprintf(stderr, fmt, ##__VA_ARGS__);                                         \
  exit(EXIT_SUCCESS);

#define usage(fmt, ...) error(fmt, ##__VA_ARGS__)

#endif
