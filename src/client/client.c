#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "color.h"
#include "input.h"
#include "linkedlist.h"
#include "salga.h"
#include "types.h"

#define AVOID_BUSY_WAIT 100000

#define PREFIX_CMD '/'
#define LIST_CMD "/list"
#define EXIT_CMD "/exit"

typedef struct {
  salgachat_pkt *pkt;
  i32 sockfd;
} thread_data;

i32 sockfd = -1;
volatile sig_atomic_t finish = false;

message *head = NULL;

char user[USERSIZE + 1];
char input[MSGSIZE + 1];

void remove_newline(char *s) { s[strlen(s) - 1] = '\0'; }

void client_sigint_handler(i32 __attribute__((unused)) sig) {
  salgachat_pkt pkt = {0};

  strncpy(pkt.user, user, USERSIZE);
  pkt.commands |= SC_EXITCMD;
  write(sockfd, &pkt, sizeof(salgachat_pkt));

  finish = true;
}

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

void *send_handler(void *arg) {
  thread_data *data = (thread_data *)arg;

  while (!finish) {
    display_messages(&head);
    printf("\nType: > %s", input);
    fflush(stdout);

    // fgets(data->pkt->msg, MSGSIZE, stdin);
    // remove_newline(data->pkt->msg);

    if (!noncanon_input(input, MSGSIZE)) {
      memset(input, 0, MSGSIZE);
      continue;
    }

    strncpy(data->pkt->msg, input, MSGSIZE);
    memset(input, 0, MSGSIZE);

    if (data->pkt->msg[0] == PREFIX_CMD) {
      if (strncmp(data->pkt->msg, LIST_CMD, strlen(data->pkt->msg)) == 0) {
        data->pkt->commands |= SC_LISTCMD;
      } else if (strncmp(data->pkt->msg, EXIT_CMD, strlen(data->pkt->msg)) ==
                 0) {
        data->pkt->commands |= SC_EXITCMD;
      }
    }

    write(data->sockfd, data->pkt, sizeof(salgachat_pkt));
    add_message(&head, data->pkt->user, data->pkt->msg);

    data->pkt->commands &= ~(SC_LISTCMD | SC_EXITCMD);
  }

  pthread_exit(NULL);
}

void *recv_handler(void *arg) {
  i32 sockfd = *((int *)arg);

  salgachat_pkt pkt = {0};

  while (!finish) {
    i64 recved = read(sockfd, &pkt, sizeof(salgachat_pkt));

    if (recved <= 0) {
      fprintf(stderr, "client: error: Server unreachable\n");
      finish = true;
      break;
    }

    add_message(&head, pkt.user, pkt.msg);
    display_messages(&head);

    printf("\nType: > %s", input);
    fflush(stdout);
  }

  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  signal(SIGINT, client_sigint_handler);

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

  struct style sty = {WHITE, DEFAULT(BG), BOLD};
  printfc(&sty, SALGABANNER);

  salgachat_pkt pkt = {0};
  printf("Enter a user: ");
  fgets(pkt.user, USERSIZE, stdin);

  if (pkt.user[0] == '\n') {
    error("client: error: Name must be specified\n");
  }

  remove_newline(pkt.user);
  pkt.flags |= SF_CONNECTING;

  strncpy(user, pkt.user, USERSIZE);

  if (connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) == -1) {
    error("client: error: %s\n", strerror(errno));
  }

  write(sockfd, &pkt, sizeof(salgachat_pkt));
  pkt.flags &= ~SF_CONNECTING;

  salgachat_pkt srv_pkt = {0};
  read(sockfd, &srv_pkt, sizeof(salgachat_pkt));

  if (srv_pkt.flags & SF_UNAVAILABLE) {
    error("client: error: Max clients has been reached\n");
  }

  pthread_t tid1, tid2;
  thread_data data = {&pkt, sockfd};

  i32 res;
  if ((res = pthread_create(&tid1, NULL, send_handler, (void *)&data)) != 0) {
    error("client: error: %s\n", strerror(res));
  }

  if ((res = pthread_create(&tid2, NULL, recv_handler, (void *)&sockfd)) != 0) {
    error("client: error: %s\n", strerror(res));
  }

  while (!finish) {
    usleep(AVOID_BUSY_WAIT);
  }

  pthread_cancel(tid1);
  pthread_cancel(tid2);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  printf("\nClient finished...\n");

  close(sockfd);
  return EXIT_FAILURE;
}
