/*
 *Grupo 61
 *Matheus Nunes fc47883
 *Pedro Marques fc48674
 *Rodrigo Barrocas fc53680
 */

#include "data.h"
#include "entry.h"
#include "sdmessage.pb-c.h"
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

/* Função para estabelecer uma associação entre o cliente e o servidor,
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtree_t *rtree_connect(const char *address_port)
{
    char *hostname = strtok((char *)address_port, ":");
    short porto = htons(atoi(strtok(NULL, "")));
    struct rtree_t *rtree = malloc(sizeof(struct rtree_t));
    if (rtree == NULL)
    {
        return NULL;
    }

    // Criar socket
    if (rtree->socket = socket(AF_INET, SOCK_STREAM, 0) < 0)
    {
        printf("Erro ao criar socket TCP");
        return NULL;
    }

    // Preencher estrutura para a conexao
    rtree->server.sin_family = AF_INET;
    rtree->server.sin_port = porto;
    if (inet_pton(AF_INET, hostname, &(rtree->server.sin_addr)) < 1)
    {
        printf("Erro ao converter IP\n");
        close(rtree->socket);
        free(rtree);
        return NULL;
    }

    // Estabelecer conexao com o servidor
    if (network_connect(rtree) == 1)
    {
        return NULL;
    }

    return rtree;
}

/* Termina a associação entre o cliente e o servidor, fechando a
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtree_disconnect(struct rtree_t *rtree)
{
    close(rtree->socket);
    free(rtree);
    return 0;
}

/* Função para adicionar um elemento na árvore.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtree_put(struct rtree_t *rtree, struct entry_t *entry)
{
    struct message_t *msg = malloc(sizeof(struct message_t));
    if (msg == NULL)
    {
        return -1;
    }
    struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
    if (msgGRANDE == NULL)
    {
        return -1;
    }
    message_t__init(msgGRANDE);
    msg->msgConvert = msgGRANDE;
    msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_PUT;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_ENTRY;
    msg->msgConvert->data = entry->value->data;
    msg->msgConvert->data_size = entry->value->datasize;
    msg->msgConvert->key = entry->key;
    msg = network_send_receive(rtree, msg);
    if (msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR)
    {
        printf("Erro a inserir a entrada!\n");
        message_t__free_unpacked(msg->msgConvert, NULL);
        free(msg);
        return -1;
    }
    free(msgGRANDE);
    message_t__free_unpacked(msg->msgConvert, NULL);
    free(msg);

    // printf("Entrada inserida com sucesso\n");
    return 0;
}

/* Função para obter um elemento da árvore.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtree_get(struct rtree_t *rtree, char *key)
{
    struct message_t *msg = malloc(sizeof(struct message_t));
    if (msg == NULL)
    {
        return NULL;
    }
    struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
    if (msgGRANDE == NULL)
    {
        return NULL;
    }
    message_t__init(msgGRANDE);
    msg->msgConvert = msgGRANDE;
    msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_GET;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_KEY;
    msg->msgConvert->key = key;
    if (msg->msgConvert->key == NULL)
    {
        free(msg->msgConvert->key);
        return NULL;
    }
    msg = network_send_receive(rtree, msg);
    if (msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR)
    {
        printf("Erro a obter elemento da arvore\n");
        message_t__free_unpacked(msg->msgConvert, NULL);
        free(msg);
        return NULL;
    }
    // printf("Vitoria a obter elemento da arvore\n");
    struct data_t *dataConvert = data_create2(msg->msgConvert->data_size, msg->msgConvert->data);
    msg->msgConvert->data = NULL;
    message_t__free_unpacked(msg->msgConvert, NULL);
    free(msgGRANDE);
    free(msg);
    return dataConvert;
}

/* Função para remover um elemento da árvore. Vai libertar
 * toda a memoria alocada na respetiva operação rtree_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtree_del(struct rtree_t *rtree, char *key)
{
    struct message_t *msg = malloc(sizeof(struct message_t));
    if (msg == NULL)
    {
        return -1;
    }
    struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
    if (msgGRANDE == NULL)
    {
        return -1;
    }
    message_t__init(msgGRANDE);
    msg->msgConvert = msgGRANDE;
    msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_DEL;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_KEY;
    msg->msgConvert->key = key;
    if (msg->msgConvert->key == NULL)
    {
        return -1;
    }
    msg = network_send_receive(rtree, msg);
    if (msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR)
    {
        printf("Erro a eliminar elemento da arvore\n");
        message_t__free_unpacked(msg->msgConvert, NULL);
        free(msg);
        return -1;
    }
    message_t__free_unpacked(msg->msgConvert, NULL);
    free(msgGRANDE);
    free(msg);
    // printf("Vitoria a eliminar elemento da arvore\n");
    return 0;
}

/* Devolve o número de elementos contidos na árvore.
 */
int rtree_size(struct rtree_t *rtree)
{
    struct message_t *msg = malloc(sizeof(struct message_t));
    if (msg == NULL)
    {
        return -1;
    }
    struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
    if (msgGRANDE == NULL)
    {
        return -1;
    }
    message_t__init(msgGRANDE);
    msg->msgConvert = msgGRANDE;
    msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_SIZE;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;
    msg = network_send_receive(rtree, msg);
    if (msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR)
    {
        printf("Erro a obter numero de elementos da arvore\n");
        message_t__free_unpacked(msg->msgConvert, NULL);
        free(msg);
        return -1;
    }
    int size = msg->msgConvert->sizeofkeys;
    message_t__free_unpacked(msg->msgConvert, NULL);
    free(msgGRANDE);
    free(msg);
    // printf("Vitoria a obter numero de elementos da arvore\n");
    return size;
}

/* Função que devolve a altura da árvore.
 */
int rtree_height(struct rtree_t *rtree)
{
    struct message_t *msg = malloc(sizeof(struct message_t));
    if (msg == NULL)
    {
        return -1;
    }
    struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
    if (msgGRANDE == NULL)
    {
        return -1;
    }
    message_t__init(msgGRANDE);
    msg->msgConvert = msgGRANDE;
    msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_HEIGHT;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;
    msg = network_send_receive(rtree, msg);
    if (msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR)
    {
        printf("Erro a obter a altura da arvore\n");
        message_t__free_unpacked(msg->msgConvert, NULL);
        free(msg);
        return -1;
    }
    int size = msg->msgConvert->sizeofkeys;
    message_t__free_unpacked(msg->msgConvert, NULL);
    free(msgGRANDE);
    free(msg);
    // printf("Vitoria a obter numero de elementos da arvore\n");
    return size;
}

/* Devolve um array de char* com a cópia de todas as keys da árvore,
 * colocando um último elemento a NULL.
 */
char **rtree_get_keys(struct rtree_t *rtree)
{
    struct message_t *msg = malloc(sizeof(struct message_t));
    if (msg == NULL)
    {
        return NULL;
    }
    struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
    if (msgGRANDE == NULL)
    {
        free(msg);
        return NULL;
    }
    message_t__init(msgGRANDE);
    msg->msgConvert = msgGRANDE;
    msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_NONE;

    msg = network_send_receive(rtree, msg);
    if (msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR)
    {
        printf("Erro a obter keys da arvore\n");
        message_t__free_unpacked(msg->msgConvert, NULL);
        free(msgGRANDE);
        free(msg);
        return NULL;
    }
    char **keys = msg->msgConvert->keys;
    msg->msgConvert->keys = NULL;
    message_t__free_unpacked(msg->msgConvert, NULL);
    free(msgGRANDE);
    free(msg);
    return keys;
}

/* Devolve um array de void* com a cópia de todas os values da árvore,
 * colocando um último elemento a NULL.
 */
void **rtree_get_values(struct rtree_t *rtree)
{
    struct message_t *msg = malloc(sizeof(struct message_t));
    if (msg == NULL)
    {
        return NULL;
    }
    struct _MessageT *msgGRANDE = malloc(sizeof(struct _MessageT));
    if (msgGRANDE == NULL)
    {
        free(msg);
        return NULL;
    }
    message_t__init(msgGRANDE);
    msg->msgConvert = msgGRANDE;
    msg->msgConvert->opcode = MESSAGE_T__OPCODE__OP_GETVALUES;
    msg->msgConvert->c_type = MESSAGE_T__C_TYPE__CT_VALUES;

    msg = network_send_receive(rtree, msg);
    if (msg->msgConvert->opcode == MESSAGE_T__OPCODE__OP_ERROR)
    {
        printf("Erro a obter keys da arvore\n");
        message_t__free_unpacked(msg->msgConvert, NULL);
        free(msgGRANDE);
        free(msg);
        return NULL;
    }
    char **values = msg->msgConvert->values;
    msg->msgConvert->values = NULL;
    message_t__free_unpacked(msg->msgConvert, NULL);
    free(msgGRANDE);
    free(msg);
    return values;
}
