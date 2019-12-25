#include "tcp_util.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#pragma comment(lib, "ws2_32.lib")

void win_socket_init()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        _Exit(err);
    }
}

void tcp_error(const char *msg)
{
    fprintf(stderr, "%s: err: %d\n", msg, WSAGetLastError());
    _Exit(1);
}

direction_msg init_direction_msg(int direction)
{
    direction_msg msg;

    msg.direction = direction;
    msg.magic = DIRECTION_MSG_MAGIC;

    return msg;
}

int inet_pton(int af, const char *src, void *dst)
{
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN+1];

    ZeroMemory(&ss, sizeof(ss));
    /* stupid non-const API */
    strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
        switch(af) {
            case AF_INET:
                *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
                return 1;
            case AF_INET6:
                *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
                return 1;
            default:
                return -1;
        }
    }
    return 0;
}


struct sockaddr_in parse_address(const char *ip_address, int port)
{
    struct sockaddr_in addr;
    int rc;

    memset((char *) &addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    rc = inet_pton(AF_INET, ip_address, &addr.sin_addr);
    if (rc <= 0) {
        fprintf(stderr,"ERROR, failed to construct ip from %s\n", ip_address);
        exit(0);
    }
    addr.sin_port = htons(port);

    return addr;
}