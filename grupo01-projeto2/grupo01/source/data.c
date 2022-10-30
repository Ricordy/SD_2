/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include <stdlib.h>
#include <string.h>
#include "data.h"

/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size
 */
struct data_t *data_create(int size){
  if(size<=0){
    return NULL;
  }
  struct data_t *data = malloc(sizeof(struct data_t));
  if(data == NULL){
    return NULL;
  }
  data->data = malloc(size);
  if(data->data == NULL){
      return NULL;
  }
  data->datasize = size;
  return data;
}

/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data.
 */
struct data_t *data_create2(int size, void *data){
  if(data == NULL || size<=0){
    return NULL;
  }
  struct data_t *newData = malloc(sizeof(struct data_t));
  if(newData == NULL){
    return NULL;
  }
  newData->data = data;
  newData->datasize = size;
  return newData;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 */
void data_destroy(struct data_t *data){
  if(data == NULL || data->data == NULL) return;
  free(data->data);
  free(data);
}

/* Função que duplica uma estrutura data_t, reservando a memória
 * necessária para a nova estrutura.
 */
struct data_t *data_dup(struct data_t *data){
  if(data == NULL || data->datasize <= 0 || data->data == NULL){
    return NULL;
  }
  struct data_t *newData = malloc(sizeof(struct data_t));
  if(newData == NULL){
    return NULL;
  }
  newData->data = malloc(data->datasize);
  memcpy(newData->data, data->data, data->datasize);
  newData->datasize = data->datasize;
	return newData;
}
