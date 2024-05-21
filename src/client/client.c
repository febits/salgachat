#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "salga.h"
#include "types.h"

i32 sockfd = -1;

i32 getipbyname(const char *name, u32 *dst) {
  struct addrinfo hint = {0};
  struct addrinfo *result = NULL;

  hint.ai_family = AF_INET;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_protocol = 0;

  i32 status = getaddrinfo(name, NULL, &hint, &result);
  if (status != 0) {
    return status;
  }

  struct sockaddr_in *addr = (struct sockaddr_in *)result->ai_addr;
  *dst = addr->sin_addr.s_addr;

  freeaddrinfo(result);
  return 0;
}

int main(int argc, char **argv) {

  if (argc != 3) {
    usage("client: Usage: %s [host] [port]\n", argv[0]);
  }

  char *host = argv[1];
  u16 port = atoi(argv[2]);
  u32 ip = 0;
  i32 s = 0;

  if ((s = getipbyname(host, &ip)) != 0) {
    error("client: error: %s\n", gai_strerror(s));
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    error("client: error: %s\n", strerror(errno));
  }

  struct sockaddr_in srvaddr = {0};

  srvaddr.sin_family = AF_INET;
  srvaddr.sin_addr.s_addr = ip;
  srvaddr.sin_port = htons(port);

  if (connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) == -1) {
    error("client: error: %s\n", strerror(errno));
  }

  printf("connected\n");

  return EXIT_SUCCESS;
}
