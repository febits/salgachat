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

u8 clients_counter = 0;

int main(int argc, char **argv) {
  loguva_add_stream(stdout);

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
  inet_ntop(AF_INET, &srvaddr.sin_addr, strip, INET_ADDRSTRLEN);

  printf(SALGABANNER "[<>] Listening on %s:%u\n\n", strip, port);

  struct sockaddr_in cltaddr = {0};
  socklen_t cltsize = sizeof(cltaddr);

  while (true) {
    salgachat_pkt pkt = {0};

    i32 clientfd = accept(sockfd, (struct sockaddr *)&cltaddr, &cltsize);
    if (clientfd == -1) {
      loguva(ERROR, "server: accept error: %s\n", strerror(errno));
      continue;
    }

    salgachat_pkt srv_pkt = {0};
    if (clients_counter >= CLIENTSMAX) {
      srv_pkt.flags |= S_UNAVAILABLE;

      inet_ntop(AF_INET, &cltaddr.sin_addr, strip, INET_ADDRSTRLEN);
      loguva(INFO,
             "[<>] Connection rejected! Max clients has been reached "
             "(connection from %s)\n",
             strip);
      write(clientfd, &srv_pkt, sizeof(salgachat_pkt));

      close(clientfd);
      continue;
    }

    srv_pkt.flags &= ~S_UNAVAILABLE;

    write(clientfd, &srv_pkt, sizeof(salgachat_pkt));
    read(clientfd, &pkt, sizeof(salgachat_pkt));

    if (pkt.flags & S_CONNECTING) {
      inet_ntop(AF_INET, &cltaddr.sin_addr, strip, INET_ADDRSTRLEN);
      loguva(INFO, "[+] %s has connected (connection from %s)\n", pkt.user,
             strip);
    }

    printf("connected - %u\n", clients_counter++);
    close(clientfd);
  }

  close(sockfd);
  return EXIT_SUCCESS;
}
