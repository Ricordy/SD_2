#ifndef _TREE_SKEL_H
#define _TREE_SKEL_H

#include "sdmessage.pb-c.h"
#include "tree.h"

struct tree_t *tree; 

/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). 
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init(){
    if(tree = tree_create() == NULL)
        return -1;
    return 0;
}
    

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy(){
    tree_destroy(tree);
}

/* Executa uma operação na árvore (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, árvore nao incializada)
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
    msg->msgConvert->sizeofkeys = tree_size(tree);
    return 0;
  }else if(operacao == MESSAGE_T__OPCODE__OP_DEL){
    //################################################
    //del
    msg->msgConvert->opcode +=1;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;
    if(conteudo == MESSAGE_T__C_TYPE__CT_KEY ){
      if(tree_del(tree,msg->msgConvert->key)==-1){
        msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_ERROR;
        return -1;
      }
    }else if(conteudo == MESSAGE_T__C_TYPE__CT_ENTRY){
      if(tree_del(tree,msg->msgConvert->key)==-1){
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
       if((dataReceived = tree_get(tree,msg->msgConvert->key))!=NULL){
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
       if((dataReceived = tree_get(tree,msg->msgConvert->key))!=NULL){
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
    if((tree_put(tree,msg->msgConvert->key,dataColocar))==-1){
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
    if((keys=tree_get_keys(tree))==NULL){
      msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_ERROR;
      msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;
      return -1;
    }
    msg->msgConvert->keys = keys;
    msg->msgConvert->n_keys = tree_size(tree);
    return 0;
    

  }
  return 0;
}

#endif
