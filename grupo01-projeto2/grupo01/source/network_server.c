/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include <signal.h>
#include "network_server.h"
#include "inet.h"
#include "message-private.h"

//socket inicial
int sockfd;
struct sockaddr_in server,client;
char str[MAX_MSG+1];
int nbytes, count;
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
  int connsockfd;
  struct message_t *msg;


  while((connsockfd = accept(listening_socket,(struct sockaddr *)&client, &size_client))>0){
    printf("Recebeu cliente!\n");
    if(connsockfd==-1){
      perror("Error no accept!\n");
      return -1;
    }

    while(1){
      msg = network_receive(connsockfd);
      if(msg == NULL){
        message_t__free_unpacked(msg->msgConvert,NULL);
        free(msg);
        close(connsockfd);
        printf("Erro a receber mensagem!\n");
        break;
      }
      int inv = invoke(msg);
      if(inv == -1){
        message_t__free_unpacked(msg->msgConvert,NULL);
        free(msg);
        close(connsockfd);
        printf("Erro invoke!\n");
        break;
      }
      if((network_send(connsockfd,msg)==-1)){
        message_t__free_unpacked(msg->msgConvert,NULL);
        free(msg);
        close(connsockfd);
        printf("Erro a enviar mensagem!\n");
        break; 
      }else{
        printf("Mensagem enviada!\n");
      }
      
    }

  }
  message_t__free_unpacked(msg->msgConvert,NULL);
  free(msg->msgConvert);
  free(msg);
  close(connsockfd);
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
  msgRecebida->msgConvert = malloc(sizeof(struct _MessageT));
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
