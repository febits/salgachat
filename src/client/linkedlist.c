#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"
#include "salga.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
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

void display_messages(message *head) {
  pthread_mutex_lock(&lock);

  printf("\x1b[H\x1b[J");
  message *curr = head;
  for (; curr; curr = curr->next) {
    printf("[%s]: %s\n", curr->user, curr->msg);
  }

  pthread_mutex_unlock(&lock);
}
