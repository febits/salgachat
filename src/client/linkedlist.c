#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "linkedlist.h"
#include "salga.h"

#define MAX_PRINTABLE_MSG 100

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
u8 message_counter = 0;

void add_message(message **head, const char *user, const char *msg) {
  pthread_mutex_lock(&lock);

  message *node = malloc(sizeof(message));
  strncpy(node->user, user, USERSIZE);
  strncpy(node->msg, msg, MSGSIZE);
  node->next = NULL;

  if (*head == NULL) {
    *head = node;
  } else {
    message *curr = *head;
    for (; curr->next; curr = curr->next) {
      ;
    }
    curr->next = node;
  }

  message_counter++;

  pthread_mutex_unlock(&lock);
}

static void cleanup_messages(message **head) {
  message *curr = *head;

  for (u8 i = 0; i < (MAX_PRINTABLE_MSG / 2); i++) {
    message *tmp = curr;
    curr = curr->next;
    free(tmp);
    message_counter--;
  }

  *head = curr;
}

void display_messages(message **head) {
  pthread_mutex_lock(&lock);

  if (message_counter == MAX_PRINTABLE_MSG) {
    cleanup_messages(head);
  }

  printf("\x1b[H\x1b[J");
  message *curr = *head;
  struct style s_clients = {WHITE, DEFAULT(BG), DIM | BOLD};
  struct style s_server = {BRIGHT_WHITE, BRIGHT_RED, DIM | BOLD};

  for (; curr; curr = curr->next) {
    printfc(strncmp(curr->user, "Server", strlen(curr->user)) == 0 ? &s_server
                                                                   : &s_clients,
            "[%s]", curr->user);
    printf(": %s\n", curr->msg);
  }

  pthread_mutex_unlock(&lock);
}
