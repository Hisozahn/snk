/*
 * tcpclient.c - A simple TCP client
 * usage: tcpclient <ip_address> <port>
 */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "tcp_util.h"
#pragma comment(lib, "ws2_32.lib")

#define BUFSIZE 1024

int tcp_client_main(const struct sockaddr_in *serveraddr) {
    int sockfd, n;
    char buf[BUFSIZE];

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        tcp_error("ERROR opening socket");

    /* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr *)serveraddr, sizeof(*serveraddr)) < 0)
        tcp_error("ERROR connecting");

    /* get message line from the user */
    printf("Please enter msg: ");
    memset(buf, 0, BUFSIZE);
    fgets(buf, BUFSIZE, stdin);

    /* send the message line to the server */
    n = send(sockfd, buf, strlen(buf), 0);
    if (n < 0)
        tcp_error("ERROR writing to socket");

    /* print the server's reply */
    memset(buf, 0, BUFSIZE);
    n = recv(sockfd, buf, BUFSIZE, 0);
    if (n < 0)
        tcp_error("ERROR reading from socket");
    printf("Echo from server: %s", buf);
    close(sockfd);
    return 0;
}

int tcp_server_main(const struct sockaddr_in *serveraddr) {
    int sockfd;
    int new_socket;
    char buf[BUFSIZE];
    int addrlen = sizeof(*serveraddr);
    int rc;
    char *hello = "hello from server";

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        tcp_error("ERROR opening socket");

    if ((rc = bind(sockfd, (struct sockaddr *)serveraddr, sizeof(*serveraddr))) < 0)
        tcp_error("ERROR binding socket");

    if (listen(sockfd, 3) < 0)
        tcp_error("ERROR listening on socket");

    if ((new_socket = accept(sockfd, (struct sockaddr *)serveraddr, (socklen_t *)&addrlen)) < 0)
        tcp_error("ERROR accepting on socket");

    rc = recv( new_socket , buf, sizeof(buf), 0);
    if (rc < 0)
        tcp_error("ERROR reading from socket");

    printf("server got: %s\n", buf);
    send(new_socket , hello , strlen(hello) , 0);

    close(new_socket);
    close(sockfd);

    return 0;
}