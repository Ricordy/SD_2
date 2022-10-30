/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include <stdlib.h>
#include <string.h>
#include "table-private.h"
#include "list-private.h"
#include "table.h"

/* Funcao hash auxiliar
 *
 */
  int key_hash(char *key, int l){
    if(key == NULL)
		  return -1;
    if(l < 1)
		  return -1;
	  int soma = 0;
	  int i;
    for(i=0; key[i]!='\0'; i++){
      soma += key[i];
    }
    return (soma % l);
}


/* Função para criar/inicializar uma nova tabela hash, com n
 * linhas (n = módulo da função hash)
 * Em caso de erro retorna NULL.
 */
 struct table_t *table_create(int n){
   if(n<= 0){
     return NULL;
   }
   struct table_t *table = malloc(sizeof(struct table_t));
   if(table == NULL)
     return NULL;

   table->numList = n;
   table->numElements = 0;
   table->lists = malloc(sizeof(struct list_t *)*n);
   if(table->lists==NULL){
     free(table);
     return NULL;
   }
   for(int index=0; index<n; index++)
     table->lists[index] = list_create();

   return table;
 }

/* Função para libertar toda a memória ocupada por uma tabela.
 */
void table_destroy(struct table_t *table){
  if(table == NULL)
    return;

  for(int index=0; index<table->numList; index++){
    list_destroy(table->lists[index]);
  }
  free(table->lists);
  free(table);
}

/* Função para adicionar um par chave-valor à tabela.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na tabela,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int table_put(struct table_t *table, char *key, struct data_t *value){

  if(table == NULL || key == NULL || value == NULL)
    return -1;
  int numeroEntrada = key_hash(key,table->numList);
  struct entry_t *entryNova = entry_create(strdup(key),data_dup(value));
  int sizeLista = list_size(table->lists[numeroEntrada]);
  list_add(table->lists[numeroEntrada],entryNova);
  int sizeListaNova = list_size(table->lists[numeroEntrada]);
  if(sizeLista != sizeListaNova){
    table->numElements++;
  }
	return 0;

}

/* Função para obter da tabela o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser
 * libertados no contexto da função que chamou table_get, ou seja, a
 * função aloca memória para armazenar uma *CÓPIA* dos dados da tabela,
 * retorna o endereço desta memória com a cópia dos dados, assumindo-se
 * que esta memória será depois libertada pelo programa que chamou
 * a função.
 * Devolve NULL em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key){
  if(table == NULL || key == NULL)
    return NULL;
  int numeroEntrada = key_hash(key,table->numList);
  struct entry_t *entry = list_get(table->lists[numeroEntrada],key);
  if(entry == NULL || entry->value == NULL){
    return NULL;
  }
  struct data_t *copia = data_dup(entry->value);
	return copia;
}

/* Função para remover um elemento da tabela, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação table_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int table_del(struct table_t *table, char *key){
  if(table == NULL || key == NULL)
    return -1;

  int numeroEntrada = key_hash(key,table->numList);
  int result = list_remove(table->lists[numeroEntrada],key);
  if(result == 0){
    table->numElements--;
    return 0;
  }
	return -1;
}

/* Função que devolve o número de elementos contidos na tabela.
 */
int table_size(struct table_t *table){
  if (table == NULL)
    return -1;
  return table->numElements;
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **table_get_keys(struct table_t *table){

  if(table == NULL)
    return NULL;

  char **listaKeys = malloc((table->numElements+1)*(sizeof(char*)+1));
  if(listaKeys == NULL)
    return NULL;
  int index = 0;
  struct list_t **vetor = table->lists;
  for(int i=0;i<table->numList;i++){
    char** listaKeysAux = list_get_keys(vetor[i]);
    for(int j=0; j<vetor[i]->size; j++){
      listaKeys[index] = listaKeysAux[j];
      index++;
    }
    free(listaKeysAux);
  }
  listaKeys[index++] = NULL;
  return listaKeys;


}

/* Função que liberta toda a memória alocada por table_get_keys().
 */
void table_free_keys(char **keys){
  list_free_keys(keys);
}
