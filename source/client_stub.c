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


/* Função para estabelecer uma associação entre o cliente e o servidor, 
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtree_t *rtree_connect(const char *address_port){
    char* hostname = strtok((char*) address_port, ":");
    short porto = htons(atoi(strtok(NULL,"")));
    struct rtree_t *rtree = malloc(sizeof(struct rtree_t));
    if(rtree == NULL){
        return NULL;
    }

    // Criar socket
    if(rtree->socket = socket(AF_INET,SOCK_STREAM,0)<0){
        perror("Erro ao criar socket TCP");
        return NULL;
    }

    //Preencher estrutura para a conexao
    rtree->server.sin_family = AF_INET;
    rtree->server.sin_port = porto;
    if(inet_pton(AF_INET,hostname,&(rtree->server.sin_addr))<1){
        printf("Erro ao converter IP\n");
        close(rtree->socket);
        free(rtree);
        return NULL;
    }

    // Estabelecer conexao com o servidor
    if(network_connect(rtree) == 1){
        return NULL;
    }

    return rtree;

}

/* Termina a associação entre o cliente e o servidor, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtree_disconnect(struct rtree_t *rtree){
    close(rtree->socket);
    free(rtree);
    return 0;
}

/* Função para adicionar um elemento na árvore.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtree_put(struct rtree_t *rtree, struct entry_t *entry){
    
}

/* Função para obter um elemento da árvore.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtree_get(struct rtree_t *rtree, char *key);

/* Função para remover um elemento da árvore. Vai libertar 
 * toda a memoria alocada na respetiva operação rtree_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtree_del(struct rtree_t *rtree, char *key);

/* Devolve o número de elementos contidos na árvore.
 */
int rtree_size(struct rtree_t *rtree);

/* Função que devolve a altura da árvore.
 */
int rtree_height(struct rtree_t *rtree);

/* Devolve um array de char* com a cópia de todas as keys da árvore,
 * colocando um último elemento a NULL.
 */
char **rtree_get_keys(struct rtree_t *rtree);

/* Devolve um array de void* com a cópia de todas os values da árvore,
 * colocando um último elemento a NULL.
 */
void **rtree_get_values(struct rtree_t *rtree);



