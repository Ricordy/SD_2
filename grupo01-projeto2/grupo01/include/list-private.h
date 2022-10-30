/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#ifndef _LIST_PRIVATE_H
#define _LIST_PRIVATE_H

#include "list.h"

struct node_t {
  struct entry_t *entry;
  struct node_t *next;
};

struct list_t {
  struct node_t *first;
  struct node_t *last;
  int size;
};

void list_print(struct list_t* list);

#endif
