/*
*Grupo 61
*Matheus Nunes fc47883
*Pedro Marques fc48674
*Rodrigo Barrocas fc53680
*/

#ifndef _TREE_PRIVATE_H
#define _TREE_PRIVATE_H

#include "tree.h"

struct tree_t {
	struct entry_t *root;
	struct tree_t *left;
	struct tree_t *right;

};


#endif
