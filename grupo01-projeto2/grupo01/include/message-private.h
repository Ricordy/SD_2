#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

struct message_t {
  struct _MessageT *msgConvert;
};

int write_all(int socket,void *buf,int len);
int read_all(int socket,void *buf,int len);

#endif
