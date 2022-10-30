/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "entry.h"
#include "serialization.h"
#include "table.h"


/* Serializa uma estrutura data num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho do buffer
 * alocado ou -1 em caso de erro.
 */
int data_to_buffer(struct data_t *data, char **data_buf){
  if(data == NULL || data_buf == NULL)
    return -1;
  char *buf = malloc(data->datasize + sizeof(int));
  if(buf == NULL)
    return -1;
  int offset = 0;
  memcpy(buf, &(data->datasize),sizeof(int));
	offset = offset + sizeof(int);
  memcpy(buf + offset, data->data ,data->datasize);
  offset = offset + data->datasize;
  *data_buf = buf;
  return offset;
}


/* De-serializa a mensagem contida em data_buf, com tamanho
 * data_buf_size, colocando-a e retornando-a numa struct
 * data_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct data_t *buffer_to_data(char *data_buf, int data_buf_size){
  if(data_buf == NULL || data_buf_size <= 0)
    return  NULL;
  struct data_t *data = data_create(data_buf_size);
  if(data == NULL)
    return NULL;
  int offset = 0;
  memcpy(&(data->datasize), data_buf ,sizeof(int));
	offset = offset + sizeof(int);
  memcpy(data->data, data_buf+offset ,data->datasize);
  return data;
}

/* Serializa uma estrutura entry num buffer que sera alocado
 * dentro da função. Além disso, retorna o tamanho deste
 * buffer alocado ou -1 em caso de erro.
 */
int entry_to_buffer(struct entry_t *data, char **entry_buf){
  if(data == NULL || entry_buf == NULL)
    return -1;
  char *buf = malloc(strlen(data->key)+1 + data->value->datasize + sizeof(int) + sizeof(int));
  if(buf == NULL)
    return -1;
  int offset = 0;
  int entry_len = (strlen(data->key)+1);
  memcpy(buf, &(entry_len),sizeof(int));
  offset = offset + sizeof(int);
  memcpy(buf + offset,data->key,entry_len);
  offset = offset + entry_len;
  memcpy(buf + offset, &(data->value->datasize),sizeof(int));
	offset = offset + sizeof(int);
  memcpy(buf + offset, data->value->data ,data->value->datasize);
  offset = offset + data->value->datasize;
  *entry_buf = buf;
  return offset;
}

/* De-serializa a mensagem contida em entry_buf, com tamanho
 * entry_buf_size, colocando-a e retornando-a numa struct
 * entry_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct entry_t *buffer_to_entry(char *entry_buf, int entry_buf_size){
  if(entry_buf == NULL || entry_buf_size <= 0)
    return  NULL;
  int offset = 0;
  //ir buscar a key
  int key_len;
  memcpy(&(key_len), entry_buf, sizeof(int));
  offset = offset + sizeof(int);
  char *key = malloc(key_len);
  memcpy(key, entry_buf+offset ,key_len);
	offset = offset + key_len;

  //ir buscar a data
  int data_len;
  memcpy(&(data_len), entry_buf+offset, sizeof(int));
  offset = offset + sizeof(int);
  void *dataAux = malloc(data_len);
  memcpy(dataAux, entry_buf+offset ,data_len);

  //criar estrutura para returnar
  struct data_t *nova = malloc(sizeof(struct data_t));
  nova->datasize = data_len;
  nova->data = dataAux;
  struct entry_t *entryNova = entry_create(key,nova);
  return entryNova;


}
