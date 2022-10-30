/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include "client_stub_private.h"
#include "network_client.h"
#include "inet.h"
#include "message-private.h"
#include "sdmessage.pb-c.h"

/* Função para estabelecer uma associação entre o cliente e o servidor,
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtable_t *rtable_connect(const char *address_port){
  char* hostname = strtok((char*) address_port, ":");
  short porto = htons(atoi(strtok(NULL,"")));
  struct rtable_t *rtable = malloc(sizeof(struct rtable_t));
  if(rtable == NULL)
    return NULL;

  //socket create
  if((rtable->socket = socket(AF_INET,SOCK_STREAM,0))<0){
    perror("Erro ao criar socket TCP");
    return NULL;
  }

  //preeche estrutura para conexao
  rtable->server.sin_family = AF_INET;
  rtable->server.sin_port = porto;
  if(inet_pton(AF_INET,hostname,&(rtable->server.sin_addr))<1){
    printf("Erro ao converter IP\n");
    close(rtable->socket);
    free(rtable);
    return NULL;
  }

  //estabelece conexao com o servidor
  if(network_connect(rtable) == -1){
    return NULL;
  }
  return rtable;
}

/* Termina a associação entre o cliente e o servidor, fechando a
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtable_disconnect(struct rtable_t *rtable){
  close(rtable->socket);
  free(rtable);
  return 0;
}

/* Função para adicionar um elemento na tabela.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
 int rtable_put(struct rtable_t *rtable, struct entry_t *entry){
   struct message_t *msg = malloc(sizeof(struct message_t));
   if(msg == NULL){
     return -1;
   }
   struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
   if(msgGRANDE == NULL){
     return -1;
   }
   message_t__init(msgGRANDE);
   msg->msgConvert = msgGRANDE;
   msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_PUT;
   msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_ENTRY;
   msg->msgConvert->data = entry->value->data;
   msg->msgConvert->data_size = entry->value->datasize;
   msg->msgConvert->key = entry->key;
   msg = network_send_receive(rtable,msg);
   if(msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR){
     printf("Erro a inserir a entrada!\n");
     message_t__free_unpacked(msg->msgConvert,NULL);
     free(msg);
     return -1;
   }
   free(msgGRANDE);
   message_t__free_unpacked(msg->msgConvert,NULL);
   free(msg);
   
   //printf("Entrada inserida com sucesso\n");
   return 0;
 }

/* Função para obter um elemento da tabela.
 * Em caso de erro, devolve NULL.
 */
 struct data_t *rtable_get(struct rtable_t *rtable, char *key){
   struct message_t *msg = malloc(sizeof(struct message_t));
   if(msg == NULL){
     return NULL;
   }
   struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
   if(msgGRANDE == NULL){
     return NULL;
   } 
   message_t__init(msgGRANDE);
   msg->msgConvert = msgGRANDE;
   msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_GET;
   msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_KEY;
   msg->msgConvert->key = key;
   if(msg->msgConvert->key == NULL){
     free(msg->msgConvert->key);
     return NULL;
   }
   msg = network_send_receive(rtable,msg);
   if(msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR){
     printf("Erro a obter elemento da tabela\n");
     message_t__free_unpacked(msg->msgConvert,NULL);
     free(msg);
     return NULL;
   }
   //printf("Vitoria a obter elemento da tabela\n");
   struct data_t *dataConvert = data_create2(msg->msgConvert->data_size,msg->msgConvert->data);
   msg->msgConvert->data = NULL;
   message_t__free_unpacked(msg->msgConvert,NULL);
   free(msgGRANDE);
   free(msg);
   return dataConvert;
 }

/* Função para remover um elemento da tabela. Vai libertar
 * toda a memoria alocada na respetiva operação rtable_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtable_del(struct rtable_t *rtable, char *key){
  struct message_t *msg = malloc(sizeof(struct message_t));
  if(msg == NULL){
    return -1;
  }
  struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
  if(msgGRANDE == NULL){
    return -1;
  }
  message_t__init(msgGRANDE);
  msg->msgConvert = msgGRANDE;
  msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_DEL;
  msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_KEY;
  msg->msgConvert->key = key;
  if(msg->msgConvert->key == NULL){
    return -1;
  }
  msg = network_send_receive(rtable,msg);
  if(msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR){
    printf("Erro a eliminar elemento da tabela\n");
    message_t__free_unpacked(msg->msgConvert,NULL);
    free(msg);
    return -1;
  }
  message_t__free_unpacked(msg->msgConvert,NULL);
  free(msgGRANDE);
  free(msg);
  //printf("Vitoria a eliminar elemento da tabela\n");
  return 0;
}

/* Devolve o número de elementos contidos na tabela.
 */
int rtable_size(struct rtable_t *rtable){
  struct message_t *msg = malloc(sizeof(struct message_t));
  if(msg == NULL){
    return -1;
  }
  struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
  if(msgGRANDE == NULL){
    return -1;
  }
  message_t__init(msgGRANDE);
  msg->msgConvert = msgGRANDE;
  msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_SIZE;
  msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;
  msg = network_send_receive(rtable,msg);
  if(msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR){
    printf("Erro a obter numero de elementos da tabela\n");
    message_t__free_unpacked(msg->msgConvert,NULL);
    free(msg);
    return -1;
  }
  int size = msg->msgConvert->sizeofkeys;
  message_t__free_unpacked(msg->msgConvert,NULL);
  free(msgGRANDE);
  free(msg);
  //printf("Vitoria a obter numero de elementos da tabela\n");
  return size;
}

/* Devolve um array de char* com a cópia de todas as keys da tabela,
 * colocando um último elemento a NULL.
 */
char **rtable_get_keys(struct rtable_t *rtable){
  struct message_t *msg = malloc(sizeof(struct message_t));
  if(msg == NULL){
    return NULL;
  }
  struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
  if(msgGRANDE == NULL){
    free(msg);
    return NULL;
  }
  message_t__init(msgGRANDE);
  msg->msgConvert = msgGRANDE;
  msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
  msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;

  msg = network_send_receive(rtable,msg);
  if(msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR){
    printf("Erro a obter keys da tabela\n");
    message_t__free_unpacked(msg->msgConvert,NULL);
    free(msgGRANDE);
    free(msg);
    return NULL;
  }
  char **keys = msg->msgConvert->keys;
  msg->msgConvert->keys = NULL;
  message_t__free_unpacked(msg->msgConvert,NULL);
  free(msgGRANDE);
  free(msg);
  return keys;
}

/* Liberta a memória alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys){
  int index = 0;
  while(keys[index] != NULL){
    free(keys[index]);
    index+=1;
  }
  free(keys);
}

/* Verifica se a operação identificada por op_n foi executada.
*/
int rtable_verify(struct rtable_t *rtable, int op_n){
  return 0;
}
