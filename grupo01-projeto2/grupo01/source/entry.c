/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "entry.h"


/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados passados.
 */
struct entry_t *entry_create(char *key, struct data_t *data){
  if (key == NULL || data == NULL)
    return NULL;
  struct entry_t *entry = malloc(sizeof(struct entry_t));
  if (entry  == NULL)
    return NULL;

  entry-> key = key;
  entry-> value = data;
  return entry;
}

/* Função que inicializa os elementos de uma entrada na tabela com o
 * valor NULL.
 */
void entry_initialize(struct entry_t *entry){
  entry-> key = NULL;
  entry-> value = NULL;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry){
  if(entry == NULL)
    return;
  data_destroy(entry->value);
  free(entry->key);
  free(entry);
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t *entry){
  if(entry == NULL || entry->key== NULL || entry->value == NULL){
    return NULL;
  }
	struct entry_t *new = malloc(sizeof(struct entry_t));
  if (new == NULL)
    return NULL;
  new->key = malloc(strlen(entry->key)+1);
  strcpy(new->key, entry->key);
  new->value = data_dup(entry-> value);
  return new;
}
