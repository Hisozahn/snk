#ifndef SNK_TCP_UTIL_H
#define SNK_TCP_UTIL_H

#define DIRECTION_MSG_MAGIC 0x7423

typedef struct direction_msg {
    int magic;
    int direction;
} direction_msg;

direction_msg init_direction_msg(int direction);
struct sockaddr_in parse_address(const char *ip_address, int port);
void tcp_error(const char *msg);
void win_socket_init();

#endif //SNK_TCP_UTIL_H
