/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */


#include "table_skel.h"
#include "inet.h"

struct table_t *server_table;

/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int table_skel_init(int n_lists){
  if((server_table = table_create(n_lists))==NULL){
    return -1;
  }
  return 0;
}

/* Liberta toda a memória e recursos alocados pela função table_skel_init.
 */
void table_skel_destroy(){
    table_destroy(server_table);
}

/* Executa uma operação na tabela (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, tabela nao incializada)
*/
int invoke(struct message_t *msg){
  //buscar dados do opcode
  short operacao = msg->msgConvert->opcode;
  short conteudo = msg->msgConvert->c_type;
  //operacoes
  if(operacao == MESSAGE_T__OPCODE__OP_SIZE){
    //################################################
    //size
    msg->msgConvert->opcode +=1;
    msg->msgConvert->c_type = MESSAGE_T__OPCODE__OP_SIZE;
    msg->msgConvert->sizeofkeys = table_size(server_table);
    return 0;
  }else if(operacao == MESSAGE_T__OPCODE__OP_DEL){
    //################################################
    //del
    msg->msgConvert->opcode +=1;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;
    if(conteudo == MESSAGE_T__C_TYPE__CT_KEY ){
      if(table_del(server_table,msg->msgConvert->key)==-1){
        msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_ERROR;
        return -1;
      }
    }else if(conteudo == MESSAGE_T__C_TYPE__CT_ENTRY){
      if(table_del(server_table,msg->msgConvert->key)==-1){
        msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_ERROR;
        return -1;
      }
    }else{
      msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_ERROR;
      return -1;
    }
    return 0;
  }else if(operacao == MESSAGE_T__OPCODE__OP_GET){
    //################################################
    //get
     msg->msgConvert->opcode +=1;
     msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_VALUE;
     struct data_t *dataReceived;
     if(conteudo == MESSAGE_T__C_TYPE__CT_KEY ){
       if((dataReceived = table_get(server_table,msg->msgConvert->key))!=NULL){
        free(msg->msgConvert->key);
        msg->msgConvert->key = NULL;
        msg->msgConvert->data = dataReceived->data;
        msg->msgConvert->data_size = dataReceived->datasize;
        free(dataReceived);
        return 0;
       }
       dataReceived = data_create2(0,NULL);
       free(dataReceived);
       return 0;
     }else if(conteudo == MESSAGE_T__C_TYPE__CT_ENTRY){
       if((dataReceived = table_get(server_table,msg->msgConvert->key))!=NULL){
        free(msg->msgConvert->key);
        msg->msgConvert->data = dataReceived->data;
        msg->msgConvert->data_size = dataReceived->datasize;
        free(dataReceived);
        return 0;
       }
       dataReceived = data_create2(0,NULL);
       free(dataReceived);
       return 0;
     }else{
       msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_ERROR;
       msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;
       return -1;
     }

  }else if(operacao == MESSAGE_T__OPCODE__OP_PUT){
    //################################################
    //put
    msg->msgConvert->opcode +=1;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;
    struct data_t *dataColocar = data_create2(msg->msgConvert->data_size,msg->msgConvert->data);
    if((table_put(server_table,msg->msgConvert->key,dataColocar))==-1){
      free(dataColocar);
      msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_ERROR;
      return -1;
    }
    free(dataColocar);
    return 0;

  }else if(operacao == MESSAGE_T__OPCODE__OP_GETKEYS){
    //################################################
    //getKeys
    msg->msgConvert->opcode +=1;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_KEYS;
    char **keys;
    if((keys=table_get_keys(server_table))==NULL){
      msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_ERROR;
      msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;
      return -1;
    }
    msg->msgConvert->keys = keys;
    msg->msgConvert->n_keys = table_size(server_table);
    return 0;
    

  }
  return 0;

}

/* Verifica se a operação identificada por op_n foi executada.
*/
int verify(int op_n){
  return 0;
}