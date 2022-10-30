/*
*Grupo 61
*Matheus Nunes fc47883
*Pedro Marques fc48674
*Rodrigo Barrocas fc53680
*/

#include <unistd.h>
#include <stdio.h>
#include "message-private.h"


int write_all(int socket,void *buf,int len){
    int sizeBuffer = len;
    int n;
    while(len > 0){
        if((n = write(socket,buf,len))<0){
            printf("Nao escreveu bem!");
            return n;
        }  
        len-=n;
        buf+=n;
    }
    return sizeBuffer;
}

int read_all(int socket,void *buf,int len){
    int sizeBuffer = len;
    int n;
    while(len > 0 ){
        if((n = read(socket,buf,len)) < 0){
            printf("Nao leu bem!");
            return n;
        }
        len -= n;
        buf += n;
    }

    return sizeBuffer;
}
