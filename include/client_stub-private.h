#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include "client_stub.h"
#include <sys/socket.h>
#include <arpa/inet.h>

struct rtree_t{
    struct sockaddr_in server;
    int socket;
};

#endif