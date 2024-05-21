#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "loguva.h"
#include "salga.h"
#include "types.h"

#define CLIENTSMAX 5

int main(int argc, char **argv) {

  if (argc != 2) {
    usage("server: Usage: %s [port]\n", argv[0]);
  }

  u16 port = atoi(argv[1]);
  if (port == 0) {
    error("server: error: Invalid port number\n");
  }

  i32 sockfd = -1;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    error("server: error: %s\n", strerror(errno));
  }

  struct sockaddr_in srvaddr = {0};

  srvaddr.sin_family = AF_INET;
  srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  srvaddr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) == -1) {
    error("server: error: %s\n", strerror(errno));
  }

  if (listen(sockfd, CLIENTSMAX) == -1) {
    error("server: error: %s\n", strerror(errno));
  }

  char strip[INET_ADDRSTRLEN] = {0};
  inet_ntop(AF_INET, &srvaddr.sin_addr.s_addr, strip, INET_ADDRSTRLEN);

  printf(SALGABANNER);
  printf("[<>] Listening on %s:%u\n\n", strip, port);

  struct sockaddr_in cltaddr = {0};
  socklen_t cltsize = sizeof(cltaddr);

  while (true) {
    i32 clientfd = accept(sockfd, (struct sockaddr *)&cltaddr, &cltsize);
    if (clientfd == -1) {
      loguva(ERROR, "server: accept error: %s\n", strerror(errno));
      continue;
    }
    printf("connected\n");
    close(clientfd);
  }

  close(sockfd);
  return EXIT_SUCCESS;
}
