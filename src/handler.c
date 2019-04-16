#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#include "handler.h"
#include "shared.h"

void *handler(void *connArg)
{
    pid_t tid = syscall(SYS_gettid);

    printf("Thread started");
    fflush(stdout);
    struct connArgs *args = (struct connArgs *)connArg;

    int sock = args->sock;
    struct sockaddr_in addr = args->addr;

  
    char *connectedIP = inet_ntoa(addr.sin_addr);
    
    printf("New connection from %s on thread %d", connectedIP, tid);
    fflush(stdout);

    char filename[FILENAME_MAX];
    recv(sock, filename, FILENAME_MAX, 0);
    printf("%s\n", filename);
    fflush(stdout);

    //TODO acquire lock on file if it exists

    FILE *fp = fopen(filename, "w");
    
    if(fp == NULL)
    {
        send(sock, ERROR_FILE_OPEN, strlen(ERROR_FILE_OPEN), 0);
        printf("Exiting\n");
        fflush(stdout);
        pthread_exit(NULL);
    };

    send(sock, OK, strlen(OK), 0);
    
    int recvAmt;
    char buf[BUFSIZE];

    while((recvAmt = recv(sock, buf, BUFSIZE, 0)) > 0)
    {
        printf("starting loop");
        fflush(stdout);
        int writeAmt = fwrite(buf, sizeof(char), recvAmt, fp);
        printf("%d\n", writeAmt);
        printf("loop");
        fflush(stdout);
        //printf("%s", buf);
        //bzero(buf, BUFSIZE);
    };

    printf("Closing fp\n");
    fclose(fp);
    close(sock);
    fflush(stdout);

};