#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
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
#define NOUSER (-1)

typedef struct {
  u8 id;
  i32 sockfd;
} thread_data;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

i32 clients[CLIENTSMAX] = {NOUSER};
u8 clients_counter = 0;

void server_sigint_handler(i32 __attribute__((unused)) sig) {
  printf("\nServer finished...\n");
  exit(EXIT_FAILURE);
}

void init_clients_list(void) {
  pthread_mutex_lock(&lock);

  for (u8 i = 0; i < CLIENTSMAX; i++) {
    clients[i] = NOUSER;
  }

  pthread_mutex_unlock(&lock);
}

u8 add_client(i32 sockfd) {
  pthread_mutex_lock(&lock);

  u8 i;
  for (i = 0; i < CLIENTSMAX; i++) {
    if (clients[i] == NOUSER) {
      clients[i] = sockfd;
      break;
    }
  }

  pthread_mutex_unlock(&lock);
  return i;
}

void remove_client(u8 id) {
  pthread_mutex_lock(&lock);
  clients[id] = NOUSER;
  pthread_mutex_unlock(&lock);
}

void *client_handler(void *arg) {
  thread_data *data = (thread_data *)arg;

  salgachat_pkt pkt = {0};
  clients_counter++;

  printf("ID: %u\nSock: %u\nCounter: %u\n", data->id, data->sockfd,
         clients_counter);

  while (true) {
    sleep(1);
  }

  printf("chegando aqui\n");

  close(data->sockfd);
  free(data);

  clients_counter--;
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  signal(SIGINT, server_sigint_handler);
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

  init_clients_list();

  while (true) {
    salgachat_pkt pkt = {0};

    i32 clientfd = accept(sockfd, (struct sockaddr *)&cltaddr, &cltsize);
    if (clientfd == -1) {
      loguva(ERROR, "server: accept error: %s", strerror(errno));
      continue;
    }

    salgachat_pkt srv_pkt = {0};
    if (clients_counter >= CLIENTSMAX) {
      srv_pkt.flags |= S_UNAVAILABLE;

      inet_ntop(AF_INET, &cltaddr.sin_addr, strip, INET_ADDRSTRLEN);
      loguva(INFO,
             "[-] Connection rejected! Max clients has been reached "
             "(connection from %s)",
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
      loguva(INFO, "[+] %s has connected (connection from %s)", pkt.user,
             strip);
    }

    u8 id = add_client(clientfd);

    pthread_t tid;
    thread_data *data = malloc(sizeof(thread_data));
    if (data == NULL) {
      loguva(ERROR, "server: error: Malloc error");
      remove_client(id);
      close(clientfd);
      continue;
    }

    data->sockfd = clientfd;
    data->id = id;

    i32 res;
    if ((res = pthread_create(&tid, NULL, client_handler, (void *)data)) != 0) {
      loguva(ERROR, "server: error: %s", strerror(res));
      remove_client(id);
      close(clientfd);
      continue;
    }
  }

  close(sockfd);
  return EXIT_SUCCESS;
}
