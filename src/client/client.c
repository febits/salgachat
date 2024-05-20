#include <stdlib.h>
#include <stdlib.h>

#include "salga.h"

int main(int argc, char **argv) {
  
  if (argc != 3) {
    usage("client: Usage: %s [ip] [port]\n", argv[0]);
  }

  return EXIT_SUCCESS;
}
