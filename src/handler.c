#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "handler.h"

void *handler(void *connArg)
{
    struct connArgs *args = (struct connArgs *)connArg;

    int sock = args->sock;
    struct sockaddr_in addr = args->addr;

    pid_t tid = syscall(SYS_gettid);
    char *connectedIP = inet_ntoa(addr.sin_addr);
};