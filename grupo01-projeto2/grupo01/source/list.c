/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "entry.h"
#include "list.h"
#include "list-private.h"


/* Função que cria uma nova lista (estrutura list_t a ser definida pelo
 * grupo no ficheiro list-private.h).
 * Em caso de erro, retorna NULL.
 */
struct list_t *list_create(){
  struct list_t *list = malloc(sizeof(struct list_t));
  if(list == NULL){
    return NULL;
  }

  list->first = NULL;
  list->last = NULL;
  list->size = 0;
  return list;
}

/* Função que elimina uma lista, libertando *toda* a memoria utilizada
 * pela lista.
 */
void list_destroy(struct list_t *list){

  if(list == NULL)
    return;
  struct node_t *current = list->first;
  while(current != NULL){
    entry_destroy(current->entry);
    struct node_t *destroir = current;
    current = current->next;
    free(destroir);
  }
  free(list);

}

/* Função que adiciona no final da lista (tail) a entry passada como
* argumento caso não exista na lista uma entry com key igual àquela
* que queremos inserir.
* Caso exista, os dados da entry (value) já existente na lista serão
* substituídos pelos os da nova entry.
* Retorna 0 (OK) ou -1 (erro).
*/

int list_add(struct list_t *list, struct entry_t *entry){
  if(list == NULL || entry == NULL)
    return -1;

  //caso lista vazia
  if(list->size == 0){
    struct node_t *novoNo = malloc(sizeof(struct node_t));
    if(novoNo == NULL)
      return -1;
    novoNo->entry = entry;
    novoNo->next = NULL;
    list->first = novoNo;
    list->last = novoNo;
    list->size++;
    return 0;
  }
  struct node_t *current = list->first;
  //caso elemento igual
  while(current!= NULL){
    if(strcmp(entry->key,current->entry->key) == 0){
      entry_destroy(current->entry);
      current->entry = entry;
      return 0;
    }else{
      current = current->next;
    }
  }
  //caso de ser no final
  if(strcmp(entry->key,list->last->entry->key) == 0){
    entry_destroy(list->last->entry);
    list->last->entry = entry;
    return 0;
  }else{
    struct node_t *novoNo = malloc(sizeof(struct node_t));
    if(novoNo == NULL)
      return -1;

    novoNo->entry = entry;
    novoNo->next = NULL;
    list->last->next = novoNo;
    list->last = novoNo;
    list->size++;
    return 0;
  }

}

/* Função que elimina da lista a entry com a chave key.
 * Retorna 0 (OK) ou -1 (erro).
 */
int list_remove(struct list_t *list, char *key){

  struct node_t * current = list->first;

  if(strcmp(current->entry->key, key) == 0){
    current = list->first->next;
    entry_destroy(list->first->entry);
    free(list->first);
    list->first = current;
    list->size--;
    return 0;
  }

  for(int i = 0; i < list->size - 1; i++){
    if(current->next == list->last && strcmp(current->next->entry->key, key) == 0){
      entry_destroy(list->last->entry);
      free(list->last);
      list->last = current;
      list->last->next = NULL;
      list->size--;
      return 0;
    }
    else if(strcmp(current->next->entry->key, key) == 0){
      struct node_t * temp_node = current->next;
      current->next = temp_node->next;
      entry_destroy(temp_node->entry);
      free(temp_node);
      list->size--;
      return 0;
    }
    else{
      current = current->next;
    }
  }
  return -1;
}

/* Função que obtém da lista a entry com a chave key.
 * Retorna a referência da entry na lista ou NULL em caso de erro.
 * Obs: as funções list_remove e list_destroy vão libertar a memória
 * ocupada pela entry ou lista, significando que é retornado NULL
 * quando é pretendido o acesso a uma entry inexistente.
*/
struct entry_t *list_get(struct list_t *list, char *key){

  if(list == NULL || key == NULL || list->size == 0)
    return NULL;

  //caso em que existe
  struct node_t *current = list->first;
  while(current != NULL){
    if(strcmp(key,current->entry->key) == 0){
      return current->entry;
    }
    current = current->next;
  }
  //caso em que nao existe
  return NULL;

}

/* Função que retorna o tamanho (número de elementos (entries)) da lista,
 * ou -1 (erro).
 */
int list_size(struct list_t *list){

  if(list == NULL) return -1;
  return list->size;

}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **list_get_keys(struct list_t *list){
  if(list == NULL || list->first == NULL){
    return NULL;
  }

  char **listaKeys = malloc((list->size + 1) * (sizeof(char*)+1));
  if (listaKeys == NULL) {
    return NULL;
  }
  struct node_t *current = list->first;
  int index;
  for(index=0; index<list->size; index++){
    listaKeys[index] = strdup(current->entry->key);
    current = current->next;
  }
  listaKeys[list->size] = NULL;
  return listaKeys;

}

/* Função que liberta a memória ocupada pelo array das keys da tabela,
 * obtido pela função list_get_keys.
 */
void list_free_keys(char **keys){
  if(keys == NULL)
    return;
  int index=0;
  while(keys[index] != NULL){
    free(keys[index]);
    index++;
  }
  free(keys);

}
