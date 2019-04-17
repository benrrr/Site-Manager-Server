/*
    Ben Ryan
    C15507277
    System Software Assigment 2
    Client Application
*/

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shared.h"

int main(int argc, char * argv[])
{
    int sock;

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(7777);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    char *selections[] = {
        "/var/www/html/",
        "/var/www/html/Sales/",
        "/var/www/html/Marketing/",
        "/var/www/html/Promotions/",
        "/var/www/html/Offers/"};


    while(1)
    {
        char filename[FILENAME_MAX] = "blank";
        char savename[FILENAME_MAX];

        int selection = 0;

        struct stat buffer;

        //ask until existing file is chosen
        while(stat(filename, &buffer) != 0)
        {
            printf("\n\nEnter the path of the file you wish to upload: ");
            strcpy(filename, "../makefile");
        };

        //ask until input is in desired range
        while(selection < 1 || selection > 5)
        {
            printf("\n\nWhich directory do you wish to upload it to.(1-5)\n\
                1. Root\n\
                2. Sales\n\
                3. Marketing\n\
                4. Promotions\n\
                5. Offers\n");

            selection = 2;
        };

        printf("\n\nEnter the name it should be saved as: ");
        strcpy(savename, "makefile");

        int dirLen = strlen(selections[--selection]);
        int nameLen = strlen(filename);

        int totalLen = dirLen + nameLen;

        if(totalLen > FILENAME_MAX)
        {
            printf("Target directory + filename too long for system");
            exit(-1);
        };

        //prep target path
        char targetPath[totalLen];
        strcpy(targetPath, selections[selection]);
        strcat(targetPath, savename);

        //prep id string
        char ids[(ID_MAX * GROUP_MAX) + 2 + GROUP_MAX];
        sprintf(ids, "%i", getuid());

        //add group ids to id string
        gid_t groups[GROUP_MAX];
        int amt = getgroups(GROUP_MAX, groups);
        for(int i=0; i < amt; i++)
        {
            char gid[ID_MAX];
            sprintf(gid, " %i", groups[i]);
            strcat(ids, gid);
        };

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0)
        {
            printf("Could not connect");
            exit(-1);
        };

        char response[BUFSIZE];
        int sentAmt, recvAmt;

        sentAmt = send(sock, ids, strlen(ids), 0);
        recvAmt = recv(sock, response, BUFSIZE, 0);

        if(strcmp(response, OK) != 0)
        {
            printf("\n\nError regarding required ids: %s", response);
            exit(-1);
        };

        sentAmt = send(sock, targetPath, totalLen, 0);
        recvAmt = recv(sock, response, BUFSIZE, 0);

        if(strcmp(response, OK) != 0)
        {
            printf("\n\nError regarding target path: %s", response);
            exit(-1);
        };

        FILE *fp = fopen(filename, "r");
        char buf[BUFSIZE];
        int readAmt;

        while((readAmt = fread(buf, sizeof(char), BUFSIZE, fp)) > 0)
        {
            sentAmt = send(sock, buf, readAmt, 0);
            
            if(sentAmt != readAmt)
            {
                printf("Error sending file piece");
                break;
            };
        };

        send(sock, ENDOFFILE, strlen(ENDOFFILE), 0);
        recv(sock, response, BUFSIZE, 0);

        printf("\n\nOutcome: %s", response);

        fclose(fp);
        close(sock); 
        fflush(stdout);

        sleep(1);
    }
}