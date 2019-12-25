#ifndef SNK_TCP_UTIL_H
#define SNK_TCP_UTIL_H

#include <stdint.h>

#define DIRECTION_MSG_MAGIC 0x7423
#define DRAW_DATA_MSG_MAGIC 0xdade

#define DRAW_DATA_SIZE_MAX 800

typedef struct direction_msg {
    int magic;
    int direction;
} direction_msg;

typedef struct draw_data_msg {
    int magic;
    uint32_t draw_data_size;
    uint32_t width;
    uint32_t height;
    uint8_t draw_data[DRAW_DATA_SIZE_MAX];
} draw_data_msg;

direction_msg init_direction_msg(int direction);
void init_draw_data_msg(uint32_t size, uint32_t width, uint32_t height,
                        const uint8_t *data, draw_data_msg *msg);
struct sockaddr_in parse_address(const char *ip_address, int port);
void tcp_error(const char *msg);
void win_socket_init();

#endif //SNK_TCP_UTIL_H
