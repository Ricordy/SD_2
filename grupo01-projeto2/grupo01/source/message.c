/*
 * Grupo 01 -> Fernando Goncalves n47860; Ana Lobo n49902; Duarte Antunes n47814
 */

#include "message-private.h"
#include "inet.h"

int write_all(int socket,void *buf,int len){

    int sizeBuffer = len;
    int n;
    while(len > 0){
        if((n = write(socket,buf,len))<0){
            perror("Nao escreveu bem!");
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
    while(len > 0){
        if((n = read(socket,buf,len))<0){
            perror("Nao leu bem!");
            return n;
        }  
        len-=n;
        buf+=n;
    }
    return sizeBuffer;

}

