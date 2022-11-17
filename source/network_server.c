#include <signal.h>
#include "network_server.h"
#include "message-private.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <poll.h>
//tamanho maximo da mensagem enviada pelo cliente
#define MAX_MSG 2048
#define NFDESC 1000 // N�mero de sockets (uma para listening)
#define TIMEOUT 50 // em milisegundos

//socket inicial
struct pollfd connections[NFDESC];
int sockfd;
struct sockaddr_in server,client;
char str[MAX_MSG+1];
int nbytes, count, nfds, kfds, i;
socklen_t size_client;

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port){
  //Cria socket TCP
  if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
    perror("Erro ao criar socket!\n");
    return -1;
  }
  //Preenche estrutura server para bind
  memset(&server,0,sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  //usar funcao setsockopt para poder fazer bind a um porto anteriormento usando
  int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&enable,sizeof(enable)) < 0){
      printf("setsockopt error!\n");
      close(sockfd);
      exit(2);
  }
  //faz bind
  if(bind(sockfd,(struct sockaddr*) &server, sizeof(server))<0){
    perror("Erro ao fazer bind!\n");
    close(sockfd);
    return -1;
  }
  //faz listen
  if(listen(sockfd,0)<0){
    perror("Erro ao executar listen!\n");
    close(sockfd);
    return -1;
  }
  return sockfd;
}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket){
  struct message_t *msg;

  size_client = sizeof(struct sockaddr);
  for (i = 0; i < NFDESC; i++)
    connections[i].fd = -1;

  connections[0].fd = listening_socket; //Listening welcome socket
  connections[0].events = POLLIN;

  nfds = 1;

  while((kfds = poll(connections, nfds, 10)) >= 0){
    if(kfds > 0){
      if((connections[0].revents & POLLIN) && (nfds < NFDESC)){
        if ((connections[nfds].fd = accept(connections[0].fd, (struct sockaddr *) &client, &size_client)) > 0){ // Liga��o feita ?
          printf("Recebeu cliente!\n");
          connections[nfds].events = POLLIN; // Vamos esperar dados nesta socket
          nfds++;
        }
      }
      for (i = 1; i < nfds; i++){
        if (connections[i].revents & POLLIN){

          msg = network_receive(connections[i].fd);
          if(msg == NULL){
            close(connections[i].fd);
            connections[i].fd = -1;
            int index;
            for(int j = 1; j<nfds; j++){
              if(connections[j].fd == -1){
                index = j;
              }
            }
            while(index<nfds){
              connections[index].fd = connections[index+1].fd;
              index++;
            }
            nfds--;
            printf("Cliente desconectado!\n");
          }else{
            int inv = invoke(msg);
            if(inv == -1){
              message_t__free_unpacked(msg->msgConvert,NULL);
              free(msg);
              close(connections[i].fd);
              connections[i].fd = -1;
              nfds--;
              printf("Erro invoke!\n");
              continue;
            }
            msg->msgConvert->op_number = inv;
            if((network_send(connections[i].fd,msg)==-1)){
              message_t__free_unpacked(msg->msgConvert,NULL);
              free(msg);
              close(connections[i].fd);
              connections[i].fd = -1;
              nfds--;
              printf("Erro a enviar mensagem!\n");
              continue; 
            }else{
              printf("Mensagem enviada!\n");
            }
          }
        }
        if((connections[i].revents & POLLHUP)||(connections[i].revents & POLLERR)){
          message_t__free_unpacked(msg->msgConvert,NULL);
          free(msg);
          close(connections[i].fd);
          connections[i].fd = -1;
          nfds--;
          printf("Erro a receber mensagem!\n");
          continue;
        }  
      }
    }
  }


  
  free(msg->msgConvert);
  free(msg);
  close(listening_socket);
  return 0;
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){
  //receber tamanho buffer
  int sizeAntes;
  read_all(client_socket,&sizeAntes,sizeof(int));
  if(sizeAntes <= 0){
    printf("Erro a receber!");
    return NULL;
  }
  int size = ntohl(sizeAntes);
  uint8_t* buf = malloc(size);

  // printf("vai receber: %d %d",size,sizeAntes);

  int tamanhoRecebido;
  if((tamanhoRecebido = read_all(client_socket,buf,size)) != size){
    free(buf);
    perror("Erro ao receber dados do servidor");
    close(sockfd);
    return NULL;
  }
  struct message_t *msgRecebida = malloc(sizeof(struct message_t));
  if(msgRecebida == NULL){
    free(buf);
    free(msgRecebida);
    return NULL;
  }
  msgRecebida->msgConvert = malloc(sizeof(struct MessageT));
  if(msgRecebida->msgConvert == NULL){
    free(buf);
    message_t__free_unpacked(msgRecebida->msgConvert,NULL);
    free(msgRecebida);
    return NULL;
  }
  message_t__init(msgRecebida->msgConvert);
  msgRecebida->msgConvert = message_t__unpack(NULL,size,buf);
  free(buf);

  return msgRecebida;
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg){
  //serializar mensagem
  unsigned len;
  uint8_t *buf = NULL;
  len = message_t__get_packed_size(msg->msgConvert);
  buf = malloc(len);
  if (buf == NULL) {
    return -1;
  }
  int sizeEnviar = htonl(len);
  message_t__pack(msg->msgConvert,buf);

  //enviar tamanho buffer
  int sizeEnviado;
  if((sizeEnviado = write_all(client_socket,&sizeEnviar,sizeof(int))) != sizeof(int)){
    perror("Erro ao enviar dados ao servidor");
    close(sockfd);
    return -1;
  }
  
  //libertar memoria ocupada pela mensagem
  message_t__free_unpacked(msg->msgConvert,NULL);
  free(msg);
  
  int nbytes;
  if((nbytes = write_all(client_socket,buf,len)) != len){
    perror("Erro ao enviar dados ao servidor");
    close(sockfd);
    return -1;
  }
  
  free(buf);
  return 0;
}

/* A função network_server_close() liberta os recursos alocados por
 * network_server_init().
 */
int network_server_close(){
    return close(sockfd);
}