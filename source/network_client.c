
//#include "client_stub.h"
#include "sdmessage.pb-c.h"
#include "message-private.h"
#include "client_stub-private.h"
#include "network_client.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) a base da
 *   informação guardada na estrutura rtree;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtree;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtree_t *rtree){
    if((connect(rtree->socket,(struct sockaddr *)&(rtree->server), sizeof(rtree->server)))<0){
        perror("Erro ao conectar ao servidor");
        close(rtree->socket);
        return -1;
    }

    return 0;
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtree_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
struct message_t *network_send_receive(struct rtree_t * rtree, struct message_t *msg){
    int socket =  rtree->socket;

    int len;
    uint8_t *buf = NULL;
    len = message_t__get_packed_size(msg->msgConvert);
    buf = malloc(len);
    if(buf == NULL){
        return NULL;
    }
    int sizeEnviar = htonl(len);

    int sizeEnviado;
    if((sizeEnviado = write_all(socket,&sizeEnviar,sizeof(int))) != sizeof(int)){
        perror("Erro ao enviar dados ao servidor");
        close(socket);
        return NULL;
    }

    message_t__pack(msg->msgConvert, buf);
    int nbytes;
    if((nbytes = write_all(socket,buf,len)) != len){
        perror("Erro ao enviar dados ao servidor");
        close(socket);
        return NULL;
    }
    free(buf);

    uint32_t sizeAntes;
    read_all(socket,&sizeAntes,sizeof(int));
    if(sizeAntes <= 0){
        printf("Erro a receber!");
        return NULL;
    }
    int size = ntohl(sizeAntes);

    uint8_t *bufReceber = malloc(size);
    int tamanhoRecebido;
    if((tamanhoRecebido = read_all(socket,(char *) bufReceber, size) != size)){
        perror("Erro ao receber dados do servidor");
        close(socket);
        return NULL;
    }

    msg->msgConvert = message_t__unpack(NULL,size,bufReceber);
    free(bufReceber);
    return msg;
}

/* A função network_close() fecha a ligação estabelecida por
 * network_connect().
 */
int network_close(struct rtree_t * rtree){
    close(rtree->socket);
    free(rtree);
    return 0;
}

