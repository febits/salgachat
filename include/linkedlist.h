#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "salga.h"

typedef struct message {
  char user[USERSIZE + 1];
  char msg[MSGSIZE + 1];

  struct message *next;
} message;

void add_message(message **head, const char *user, const char *msg);
void display_messages(message *head);

#endif
