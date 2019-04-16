#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char * argv[])
{
    int sock;

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(7777);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
}