#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <sys/fsuid.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include <syslog.h>

#include "handler.h"
#include "shared.h"

void *handler(void *connArg)
{
    pid_t tid = syscall(SYS_gettid);
    struct connArgs *args = (struct connArgs *)connArg;

    int sock = args->sock;
    struct sockaddr_in addr = args->addr;

  
    char *connectedIP = inet_ntoa(addr.sin_addr);
    
    char ids[(ID_MAX * GROUP_MAX) + 2 + GROUP_MAX];
    char filename[FILENAME_MAX];

    int sentAmt, recvAmt;

    recvAmt = recv(sock, ids,(ID_MAX * GROUP_MAX) + 2 + GROUP_MAX, 0);
    sentAmt = send(sock, OK, strlen(OK), 0);
    recvAmt = recv(sock, filename, FILENAME_MAX, 0); 

    char requiredID[ID_MAX];
    if(strstr(filename, "Sales") != NULL)
    {
        strcpy(requiredID, SALESTEAM);
    }
    else if(strstr(filename, "Offers") != NULL)
    {
        strcpy(requiredID, OFFERSTEAM);
    }
    else if(strstr(filename, "Promotions") != NULL)
    {
        strcpy(requiredID, PROMOTIONSTEAM);
    }
    else if(strstr(filename, "Marketing") != NULL)
    {
        strcpy(requiredID, MARKETINGTEAM);
    }
    else
    {
        strcpy(requiredID, EMPLOYEE);
    }
    
    if(strstr(ids, requiredID) == NULL)
    {
        send(sock, ERROR_PERMISSION, strlen(ERROR_PERMISSION), 0);
        fflush(stdout);
        pthread_exit(NULL);
    }

    pid_t pid;

    pid = fork();

    if(pid > 0)
    {
        fflush(stdout);
        pthread_exit(NULL);
    };

    char *userID = strtok(ids, " ");
    setegid(atoi(requiredID));
    seteuid(atoi(userID));

    FILE *fp = fopen(filename, "w");

    if(fp == NULL)
    {
        send(sock, ERROR_FILE_OPEN, strlen(ERROR_FILE_OPEN), 0);
        fflush(stdout);
        pthread_exit(NULL);
    };

    int fd = fileno(fp);
    int retval;
    retval = flock(fd, LOCK_EX);

    if(retval != 0)
    {
        send(sock, ERROR_FILE_LOCK, strlen(ERROR_FILE_LOCK), 0);
        fflush(stdout);
        pthread_exit(NULL);
    }

    send(sock, OK, strlen(OK), 0);
    
    char buf[BUFSIZE];

    while((recvAmt = recv(sock, buf, BUFSIZE, 0)) > 0)
    {
        fflush(stdout);
        int writeAmt = fwrite(buf, sizeof(char), recvAmt, fp);
        fflush(stdout);
    };

    flock(fd, LOCK_UN);
    fclose(fp);
    close(sock);
    fflush(stdout);
};