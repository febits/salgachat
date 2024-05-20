#include <stdio.h>
#include <stdlib.h>

#include "salga.h"

int main(int argc, char **argv) {

  if (argc != 2) {
    usage("server: Usage: %s [port]\n", argv[0]);
  }

  return EXIT_SUCCESS;
}
