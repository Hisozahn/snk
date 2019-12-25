#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <errno.h>
#include <wincon.h>
#include "snk.h"
#include "snk_util.h"
#include "tcp_util.h"
#pragma comment(lib, "ws2_32.lib")

typedef struct thread_data {
    int socket;
} thread_data;

#define CHECK_RC(_call)                                             \
do                                                                  \
{                                                                   \
    if ((rc = (_call)) != 0)                                               \
    {                                                               \
        printf("line %d, failed call %s: %d\n", __LINE__, #_call, rc);      \
        _Exit(1);                                                   \
    }                                                               \
} while (0)

static int
draw_data_convert(uint8_t *draw_data, size_t size)
{
    size_t i;

    for (i = 0; i < size; i++)
    {
        //printf("%d: %d\n", i, draw_data[i]);
        switch (draw_data[i])
        {
            case SNK_POSITION_EMPTY:
                draw_data[i] = '_';
                break;
            case SNK_POSITION_OBSTACLE:
                draw_data[i] = '-';
                break;
            case SNK_POSITION_FOOD:
                draw_data[i] = '#';
                break;
            case SNK_POSITION_SNAKE:
                draw_data[i] = 'x';
                break;
            default:
                return EINVAL;
        }
    }

    draw_data[size] = '\0';

    return 0;
}

static void
draw(const uint8_t *draw_data, uint32_t width, uint32_t height)
{
    uint32_t i;
    for (i = 0; i < height; i++)
    {
        printf("%.*s\n", width, &draw_data[i * width]);
    }
    fflush(stdout);
}

static DWORD WINAPI thread(void *arg)
{
    thread_data *data = arg;
    int socket = data->socket;
    draw_data_msg msg;
    snk_score score;
    int rc;

    while (1) {
        rc = recv(socket, &msg, sizeof(msg), 0);
        if (rc < 0 || msg.magic != DRAW_DATA_MSG_MAGIC)
            tcp_error("ERROR reading from socket");

        CHECK_RC(draw_data_convert(msg.draw_data, msg.draw_data_size));
        draw(msg.draw_data, msg.width, msg.height);
    }

    return 0;
}

int
main(int argc, char *argv[])
{
    DWORD mode = 0;
    HANDLE hStdin;
    INPUT_RECORD records[16];
    DWORD size;
    direction_msg msg;
    int sockfd;
    struct sockaddr_in server_addr;
    thread_data data;
    int rc;

    win_socket_init();

    if (argc < 3)
    {
        fprintf(stderr, "usage: <IP address> <TCP port>\n");
        _Exit(1);
    }

    server_addr = parse_address(argv[1], atoi(argv[2]));
    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        tcp_error("ERROR opening socket");

    /* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        tcp_error("ERROR connecting");

    data.socket = sockfd;
    if (CreateThread(NULL, 0, thread, &data, 0, NULL) == NULL)
    {
        fprintf(stderr, "create thread failed\n");
        _Exit(1);
    }

    hStdin = GetStdHandle(STD_INPUT_HANDLE);

    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

    while(1)
    {
        if (ReadConsoleInput(hStdin, records, SNK_ARRAY_LEN(records), &size) != 0)
        {
            if (size > 0 && records[0].EventType == KEY_EVENT && records[0].Event.KeyEvent.bKeyDown)
            {
                snk_direction new_direction;
                char c = (char)tolower(records[0].Event.KeyEvent.uChar.AsciiChar);

                switch (c)
                {
                    case 'a':
                        new_direction = SNK_DIRECTION_LEFT;
                        break;
                    case 'd':
                        new_direction = SNK_DIRECTION_RIGHT;
                        break;
                    case 'w':
                        new_direction = SNK_DIRECTION_UP;
                        break;
                    case 's':
                        new_direction = SNK_DIRECTION_DOWN;
                        break;
                    case 'l':
                        puts("leave");
                        _Exit(0);
                    default:
                        continue;
                }

                msg = init_direction_msg(new_direction);
                rc = send(sockfd, &msg, sizeof(msg), 0);
                if (rc < 0)
                    tcp_error("ERROR writing to socket");
            }
        }
    }
}