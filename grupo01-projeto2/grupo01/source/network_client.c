/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include "network_client.h"
#include "inet.h"
#include "client_stub_private.h"
#include "message-private.h"

/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) a base da
 *   informação guardada na estrutura rtable;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtable;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtable_t *rtable){
  if((connect(rtable->socket,(struct sockaddr *)&(rtable->server),sizeof(rtable->server)))<0){
    perror("Erro ao conectar ao servidor");
    close(rtable->socket);
    return -1;
  }
  return 0;
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtable_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
struct message_t *network_send_receive(struct rtable_t * rtable, struct message_t *msg){
  int socket = rtable->socket;

  //tamanho a enviar
  int len;
  uint8_t *buf = NULL;
  len = message_t__get_packed_size(msg->msgConvert);
  buf = malloc(len);
  if (buf == NULL) {
    return NULL;
  }
  int sizeEnviar = htonl(len);

  //enviar tamanho buffer
  int sizeEnviado;
  if((sizeEnviado = write_all(socket,&sizeEnviar,sizeof(int))) != sizeof(int)){
    perror("Erro ao enviar dados ao servidor");
    close(socket);
    return NULL;
  }

  //enviar buffer
  message_t__pack(msg->msgConvert,buf);
  int nbytes;
  if((nbytes = write_all(socket,buf,len)) != len){
    perror("Erro ao enviar dados ao servidor");
    close(socket);
    return NULL;
  }
  free(buf);

  //printf("A espera da resposta do servidor ...\n");

  //iniciar mensagem a receber

  //receber tamanho buffer
  uint32_t sizeAntes;
  read_all(socket,&sizeAntes,sizeof(int));
  if(sizeAntes <= 0){
    printf("Erro a receber!");
    return NULL;
  }
  int size = ntohl(sizeAntes);

  //receber buffer
  uint8_t *bufReceber = malloc(size);
  int tamanhoRecebido;
  if((tamanhoRecebido = read_all(socket,(char *)bufReceber,size) != size)){
    perror("Erro ao receber dados do servidor");
    close(socket);
    return NULL;
  };
  
  msg->msgConvert = message_t__unpack(NULL,size,bufReceber);
  free(bufReceber);
  return msg;
}

/* A função network_close() fecha a ligação estabelecida por
 * network_connect().
 */
int network_close(struct rtable_t * rtable){
  close(rtable->socket);
  free(rtable);
  return 0;
}
