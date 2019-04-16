#ifndef HANDLER_H_
#define HANDLER_H_

#include <sys/socket.h>
#include <arpa/inet.h>

struct connArgs
{
    int sock;
    struct sockaddr_in addr;
};

void *handler(void *);

#endif