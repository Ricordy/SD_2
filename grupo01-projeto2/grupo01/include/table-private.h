/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "list.h"

struct table_t {
  struct list_t **lists;
  int numList;
  int numElements;

};

/* Função para criar/inicializar uma nova tabela hash, com n linhas
 * (módulo da função HASH).
 */
struct table_t *table_create(int n);

void table_print(struct table_t *table);

#endif
