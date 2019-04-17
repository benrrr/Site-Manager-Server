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


    while(1)
    {
        char filename[FILENAME_MAX] = "blank";
        char savename[FILENAME_MAX];

        int selection = 0;
        char *selections[] = {
            "/var/www/html/",
            "/var/www/html/Sales/",
            "/var/www/html/Marketing/",
            "/var/www/html/Promotions/",
            "/var/www/html/Offers/"};

        struct stat buffer;
        while(stat(filename, &buffer) != 0)
        {
            printf("Enter the path of the file you wish to upload:\n");
            scanf("%s", filename);
        };

        while(selection < 1 || selection > 5)
        {
            printf("Which directory do you wish to upload it to.\n\
                1. Root\n\
                2. Sales\n\
                3. Marketing\n\
                4. Promotions\n\
                5. Offers\n");

            scanf("%d", &selection);
        };

        printf("Enter the name it should be saved as.\n");
        scanf("%s", savename);
        printf("%s", savename);
        int dirLen = strlen(selections[--selection]);
        int nameLen = strlen(filename);

        int totalLen = dirLen + nameLen;

        if(totalLen > FILENAME_MAX)
        {
            printf("Target directory + filename too long for system");
            exit(-1);
        };

        char targetPath[totalLen];
        strcpy(targetPath, selections[selection]);
        strcat(targetPath, savename);

        char ids[(ID_MAX * GROUP_MAX) + 2 + GROUP_MAX];
        sprintf(ids, "%i", getuid());

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

        printf("\nID RESP: %s", response);

        sentAmt = send(sock, targetPath, totalLen, 0);
        recvAmt = recv(sock, response, BUFSIZE, 0);

        printf("\nFILE RESP %s", response);


        char buf[BUFSIZE];
        FILE *fp = fopen(filename, "r");
        int readAmt;

        while((readAmt = fread(buf, sizeof(char), BUFSIZE, fp)) > 0)
        {
            send(sock, buf, readAmt, 0);
            bzero(buf, BUFSIZE);
        };

        fclose(fp);
        close(sock); 
    }
}