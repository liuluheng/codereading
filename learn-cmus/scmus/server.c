#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 500

int main(int argc, char *argv[])
{
    struct addrinfo hints;

    struct addrinfo *result, *rp;
    struct sockaddr_storage peer_addr; 
    int sfd, s, nread; 
    socklen_t peer_addr_len;
    char buf[BUF_SIZE];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = 0;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    getaddrinfo(NULL, argv[1], &hints, &result);

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;

        close(sfd);
    }

    if (rp == NULL) {
        printf("Couldn't bind\n");
        exit(-1);
    }

    freeaddrinfo(result);

    for (;;) {
        peer_addr_len = sizeof(struct sockaddr_storage);
        nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *)&peer_addr, &peer_addr_len);

        if (nread == -1)
            continue;

        char host[NI_MAXHOST], service[NI_MAXSERV];

        s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);

        if (s == 0)
            printf("Received %ld bytes from %s:%s\n", (long) nread, host, service);

    }
}
