/*
    Ben Ryan
    C15507277
    System Software Assigment 2
    Server Application
*/

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
#include <stdbool.h>

#include "handler.h"
#include "shared.h"

void *handler(void *connArg)
{
    struct connArgs *args = (struct connArgs *)connArg;

    int sock = args->sock;
    struct sockaddr_in addr = args->addr;

    char *connectedIP = inet_ntoa(addr.sin_addr);
    int connectedPort = ntohs(addr.sin_port);

    syslog(LOG_INFO, "New connection: %s, %d", connectedIP, connectedPort);
    printf("New connection: %s, %d", connectedIP, connectedPort);

    char ids[(ID_MAX * GROUP_MAX) + 2 + GROUP_MAX];
    char filename[FILENAME_MAX];

    int sentAmt, recvAmt;

    recvAmt = recv(sock, ids,(ID_MAX * GROUP_MAX) + 2 + GROUP_MAX, 0);

    if(recvAmt <= 0)
    {
        send(sock, ERROR_UNKOWN, strlen(ERROR_UNKOWN), 0);
        pthread_exit(NULL);
    };

    sentAmt = send(sock, OK, strlen(OK), 0);
    recvAmt = recv(sock, filename, FILENAME_MAX, 0); 

    //check which group id is required for the target path
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
    
    char temp[strlen(ids)];
    strcpy(temp, ids);
    char *userID = strtok(temp, " ");

    //check if the user has the required group id
    printf("%s in %s", requiredID, ids);
    if(strstr(ids, requiredID) == NULL)
    {
        send(sock, ERROR_PERMISSION, strlen(ERROR_PERMISSION), 0);
        syslog(
            LOG_ALERT,
            "Permission Denied to %s trying to access %s requiring %s group id",
            userID, filename, requiredID);
        fflush(stdout);
        pthread_exit(NULL);
    }
    syslog(
        LOG_INFO,
        "Permission Granted to %s trying to access %s requiring %s group id",
        userID, filename,requiredID);


    //fork so process effective uid and gid don't get mixed between threads
    pid_t pid;
    pid = fork();

    //parent process thread can exit
    if(pid > 0)
    {
        pthread_exit(NULL);
    };

    //child process can set new effective ids
    setegid(atoi(requiredID));
    seteuid(atoi(userID));

    FILE *fp = fopen(filename, "w");

    if(fp == NULL)
    {
        syslog(LOG_ERR, "ERROR: %s", strerror(errno));
        send(sock, ERROR_FILE_OPEN, strlen(ERROR_FILE_OPEN), 0);
        fflush(stdout);
        pthread_exit(NULL);
    };

    //acquire exclusive file lock before writing anything
    int fd = fileno(fp);
    int retval;
    retval = flock(fd, LOCK_EX);

    if(retval != 0)
    {
        syslog(LOG_ERR, "ERROR: %s", strerror(errno));
        send(sock, ERROR_FILE_LOCK, strlen(ERROR_FILE_LOCK), 0);
        fflush(stdout);
        pthread_exit(NULL);
    }

    //send ok to allow client to send file data for writing
    send(sock, OK, strlen(OK), 0);
    
    char buf[BUFSIZE];

    bool error = false;
    while((recvAmt = recv(sock, buf, BUFSIZE, 0)) > 0)
    {
        //break condition
        if(strstr(buf, ENDOFFILE) != NULL)
        {
            break;
        }
        
        int writeAmt = fwrite(buf, sizeof(char), recvAmt, fp);

        if(writeAmt != recvAmt)
        {
            error = true;
            syslog(LOG_ERR, "Error occured writing piece of file");
        };
    };

    //send final outcome
    if(error == true)
    {
        send(sock, ERROR_FILE_WRITE, strlen(ERROR_FILE_WRITE), 0);
    }
    else
    {
        send(sock, OK, strlen(OK), 0);
    }
    
    flock(fd, LOCK_UN);
    fclose(fp);
    close(sock);
    fflush(stdout);
};