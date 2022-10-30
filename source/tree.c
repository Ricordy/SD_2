/*
*Grupo 61
*Matheus Nunes fc47883
*Pedro Marques fc48674
*Rodrigo Barrocas fc53680
*/
#ifndef _TREE_H
#define _TREE_H /* Módulo tree */

#include "data.h"
#include "entry.h"
#include "tree.h"
#include "tree-private.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Função para criar uma nova árvore tree vazia.
 * Em caso de erro retorna NULL.
 */
struct tree_t *tree_create(){
    struct tree_t *tree = malloc(sizeof(struct tree_t));
    if(tree == NULL){
        return NULL;
    }

    
    tree->root = NULL;
    tree->left = NULL;
    tree->right = NULL;
    
    return tree;
}

/* Função para libertar toda a memória ocupada por uma árvore.
 */
void tree_destroy(struct tree_t *tree){
    if(tree == NULL){
        return;
    }

    // Destroy Left sub-tree
    tree_destroy(tree->left);
    // Destroy Right sub-tree
    tree_destroy(tree->right);

    // Depois de percorridos os ramos da esquerda e direita
    // Destroy entry e free da tree current
    entry_destroy(tree->root);

    /* Sera necessario fazer free ao left e right?
    * free(tree->left);
    * free(tree->right);
    * Ou apenas a tree?
    */

    free(tree);
  
}

/* Função para adicionar um par chave-valor à árvore.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na árvore,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int tree_put(struct tree_t *tree, char *key, struct data_t *value){
    
    if(tree == NULL || key == NULL || value == NULL){
        return -1;
    }

    // Se for um nó com entry vazia entao adiciona a entry
    if(tree->root == NULL){
        struct entry_t *entry = entry_create(key, value);
        tree->root = entry;
        // Se der erro
        if(tree->root == NULL){
            return -1;
        }
        // OK
        return 0;
    }

    // Se for menor
    if(key < tree->root->key){
        // Se o proximo for null, cria um novo no da arvore
        if(tree->left == NULL){
            tree->left = tree_create();
        }
        tree_put(tree->left, key, value);
    } //Se for maior
    else if(key > tree->root->key){
        // Se o proximo for null, cria um novo no da arvore
        if(tree->right == NULL){
            tree->right = tree_create();
        }
        tree_put(tree->right, key, value);
    }

}

/* Função para obter da árvore o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser
 * libertados no contexto da função que chamou tree_get, ou seja, a
 * função aloca memória para armazenar uma *CÓPIA* dos dados da árvore,
 * retorna o endereço desta memória com a cópia dos dados, assumindo-se
 * que esta memória será depois libertada pelo programa que chamou
 * a função. Devolve NULL em caso de erro.
 */
struct data_t *tree_get(struct tree_t *tree, char *key){
    if(tree == NULL || key == NULL)
        return NULL;
     // Se for menor
    if(key < tree->root->key){
        // Se o proximo for null, cria um novo no da arvore
        if(tree->left->root->key == key){
            struct entry_t *entry = entry_create(key, tree->left->root->value);
            struct data_t *copia = data_dup(entry->value);
            return copia;
            

            
        } else {
            tree_get(tree->left, key);
        }
    } //Se for maior
    else if(key > tree->root->key){
        // Se o proximo for null, cria um novo no da arvore
        if(tree->right->root->key == key){
            struct entry_t *entry = entry_create(key, tree->right->root->value);
            struct data_t *copia = data_dup(entry->value);
            return copia;
        }else{
            tree_get(tree->right, key);
        }
    }

    return NULL;

}

/* Função para remover um elemento da árvore, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação tree_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int tree_del(struct tree_t *tree, char *key){
    if(tree == NULL || key == NULL){
        return -1;
    }  
     // Se for menor
    if(key < tree->root->key){
        // Se o proximo for null, cria um novo no da arvore
        if(tree->left->root->key == key){
            tree_destroy(tree->left);
            return 0;
            

            
        } else {
            tree_del(tree->left, key);
        }
    } //Se for maior
    else if(key > tree->root->key){
        // Se o proximo for null, cria um novo no da arvore
        if(tree->right->root->key == key){
            tree_destroy(tree->right);
            return 0;
        }else{
            tree_del(tree->right, key);
        }
    }

    return -1;

}

/* Função que devolve o número de elementos contidos na árvore.
 */
int tree_size(struct tree_t *tree){
    if(tree->root == NULL){
        return 0;
    }
    else{
        // Return 1 + o size da left e right sub-tree
        return (1 + tree_size(tree->left) + tree_size(tree->right));
    }
}

/* Função que devolve a altura da árvore.
 */
int tree_height(struct tree_t *tree){
    if(tree == NULL){
        return 0;
    }
    else{
        // Calcular o height da left e righr sub-trees
        int left = tree_height(tree->left);
        int right = tree_height(tree->right);

        if(left > right){
            return (left + 1);
        }
        else{
            return (right + 1);
        }

    }

}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * árvore, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária. As keys devem vir ordenadas segundo a ordenação lexicográfica das mesmas.
 */
char **tree_get_keys(struct tree_t *tree){
    if(tree == NULL)
        return NULL;
    int size = tree_size(tree);
    char **kArray = malloc((size+1)*sizeof(char*));


        // Se o proximo for null, cria um novo no da arvore
    if(tree->left->root->key != NULL){
        tree_get_keys(tree->left);
            kArray[tree_size(tree->left)] = strdup(tree->left->root->key);
        return kArray;
            
    } 

    if(tree->right->root->key != NULL){
        tree_get_keys(tree->right);
            kArray[tree_size(tree->right)] = strdup(tree->right->root->key);
        return kArray;
    }

    return NULL;
    

    
 
}




/* Função que devolve um array de void* com a cópia de todas os values da
 * árvore, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
void **tree_get_values(struct tree_t *tree){
    int size = tree_size(tree);
    void **vArray = malloc((size+1)*sizeof(char*));

    if(tree->left->root->key != NULL){
        tree_get_values(tree->left);
            vArray[tree_size(tree->left)] = tree->left->root->value;
        return vArray;
            
    } 

    if(tree->right->root->key != NULL){
        tree_get_values(tree->right);
            vArray[tree_size(tree->right)] = tree->right->root->value;
        return vArray;
    }

    return NULL;

}


/* Função que liberta toda a memória alocada por tree_get_keys().
 */
void tree_free_keys(char **keys){
    if(keys == NULL)
        return;
    int index=0;
    while(keys[index] != NULL){
        free(keys[index]);
        index++;
    }
    free(keys);
}

/* Função que liberta toda a memória alocada por tree_get_values().
 */
void tree_free_values(void **values){
    if(values == NULL)
        return;
    int index=0;
    while(values[index] != NULL){
        free(values[index]);
        index++;
    }
    free(values);
}

#endif
