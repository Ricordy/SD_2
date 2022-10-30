/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include <signal.h>
#include "network_client.h"
#include "client_stub_private.h"
#include "inet.h"

int main(int argc, char **argv){
  printf("Iniciando cliente...\n");
  struct rtable_t *rtable;
  //testInput
  if(argc != 2 || argc < 0){
    printf("Usar comando da seguinte forma: ./table-client <server>:<port>\n");
    return -1;
  }

  //conect
  rtable = rtable_connect(argv[1]);
  if(rtable == 0){
    exit(1);
  }
  //guardar logo os parametros
  char comando[MAX_MSG];
  const char divide[3] = " \n";
  char* operacao;
  char* key;
  char* dados;
  //ciclo de comandos
  while(1){
    //barra para insercao de dados
    printf("Introduza acao: ");
    if(fgets(comando, MAX_MSG, stdin) ==NULL){
      printf("Nao introduziu comando!\n");
      break;
    }
    
    operacao = strtok(comando,divide);
    if(operacao == 0){
      printf("Introduza um comando valido!\n");
      printf("Comandos validos:\n");
      printf(" size\n");
      printf(" del <key>\n");
      printf(" get <key>\n");
      printf(" put <key> <data>\n");
      printf(" getkeys\n");
      printf(" quit\n");
    }else{
      if(strcmp(operacao, "quit") == 0){
      //################################################
      //quit
      printf("Terminar ligacao...\n");
      break;
    }else if(strcmp(operacao, "size") == 0){
      //################################################
      //size
      int size = rtable_size(rtable);
      printf("Tamanho -> %d\n",size);
    }else if(strcmp(operacao, "del") == 0){
      //################################################
      //del
      key = strtok(NULL,divide);
      if(key == 0){
        printf("O comando del necessita de mais um argumento, exemplo: del <key> \n");
      }else{
        char **keys = rtable_get_keys(rtable);
        int confirm = 0;
        int index;
        int numeroEntradas = rtable_size(rtable);
        for(index=0;index!=numeroEntradas; index+=1){
          if(strcmp(keys[index],key) == 0){
            confirm = 1;
          }
        }
        if(confirm == 0){
          printf("O comando del necessita de uma key jah inserida! \n");
        }else{
          if(rtable_del(rtable,key) == -1){
            printf("Erro a apagar!\n");
          }else{
            printf("Eliminado com sucesso!\n");
          }
        }     
      }
    }else if(strcmp(operacao, "get") == 0){
      //################################################
      //get
      key = strtok(NULL,divide);
      if(key == 0){
        printf("O comando get necessita de mais um argumento, exemplo: get <key> \n");
      }else{
        char **keys = rtable_get_keys(rtable);
        int confirm = 0;
        int index;
        int numeroEntradas = rtable_size(rtable);
        for(index=0;index!=numeroEntradas; index+=1){
          if(strcmp(keys[index],key) == 0){
            confirm = 1;
          }
        }
        if(confirm == 0){
          printf("O comando get necessita de uma key jah inserida! \n");
        }else{
          struct data_t *dataReceived;
          if((dataReceived = rtable_get(rtable,key))==NULL){
            printf("Erro a dar get, numero de key invalida!\n");
          }else{
            printf("Get feito com sucesso\n");
            printf("data: %s \n",(char *)dataReceived->data);
            data_destroy(dataReceived);
          }
        }
      }
    }else if(strcmp(operacao, "put") == 0){
      //################################################
      //put
      key = strtok(NULL,divide);
      dados = strtok(NULL, "\n");
      if(dados == 0){
        printf("O comando put necessita de mais um argumento, exemplo: put <key> <data>\n");
      }else{
        struct data_t *data = data_create2(strlen(dados)+1,dados);
        struct entry_t *entry = entry_create(key,data);
        rtable_put(rtable,entry);
        free(data);
        free(entry);
        printf("Entrada inserida com sucesso!\n");
      }
      
    }else if(strcmp(operacao, "getkeys") == 0){
      //################################################
      //getkeys
      char **keys = rtable_get_keys(rtable);
      if(keys == NULL){
        printf("Erro a obter keys!\n");
      }else{
        int index;
        int numeroEntradas = rtable_size(rtable);
        for(index=0;index!=numeroEntradas; index+=1){
          printf("%s \n",keys[index]);
        }
        rtable_free_keys(keys);
      }
    }else{
      printf("Introduza um comando valido!\n");
      printf("Comandos validos:\n");
      printf(" size\n");
      printf(" del <key>\n");
      printf(" get <key>\n");
      printf(" put <key> <data>\n");
      printf(" getkeys\n");
      printf(" quit\n");
  
    }
    }
    
    
  }
  return rtable_disconnect(rtable);
}
