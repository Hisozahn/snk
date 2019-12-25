#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <errno.h>
#include <time.h>
#include <wincon.h>
#include "snk.h"
#include "snk_util.h"
#include "tcp_util.h"
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define CHECK_RC(_call)                                             \
do                                                                  \
{                                                                   \
    if ((rc = (_call)) != 0)                                               \
    {                                                               \
        printf("line %d, failed call %s: %d\n", __LINE__, #_call, rc);      \
        _Exit(1);                                                   \
    }                                                               \
} while (0)

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

static int
draw_data_convert(uint8_t *draw_data, size_t size)
{
    size_t i;

    for (i = 0; i < size; i++)
    {
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

    return 0;
}

typedef struct thread_data {
    snk_process *proc;
    HANDLE mutex;
} thread_data;

DWORD WINAPI thread(void *arg)
{
    thread_data *data = arg;
    uint8_t draw_data[2048];
    snk_score score;
    int rc;

    while (1) {
        WaitForSingleObject(data->mutex, INFINITE);
        CHECK_RC(snk_next_tick(data->proc));
        CHECK_RC(snk_get_score(data->proc, &score));
        printf("score: %u\n", score);
        CHECK_RC(snk_render(data->proc, draw_data, sizeof(draw_data)));

        CHECK_RC(draw_data_convert(draw_data, sizeof(draw_data)));

        draw(draw_data, data->proc->field.width, data->proc->field.height);
        ReleaseMutex(data->mutex);
        Sleep(200);
    }
}

int
main(int argc, char *argv[])
{
    snk_field_obstacle obstacles[] = {{{0, 0}, {5, 0}}};
    snk_position start_position = {5, 5};
    snk_process process;
    snk_field field;
    HANDLE mutex;
    thread_data data;
    int new_socket;
    direction_msg msg;
    int sockfd;
    struct sockaddr_in server_addr;
    int rc;
    int addrlen = sizeof(server_addr);

    win_socket_init();

    if (argc < 3) {
        fprintf(stderr, "arguments are invalid\n");
        _Exit(1);
    }

    server_addr = parse_address(argv[1], atoi(argv[2]));
    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        tcp_error("ERROR opening socket");

    if ((rc = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
    tcp_error("ERROR binding socket");

    if (listen(sockfd, 3) < 0)
        tcp_error("ERROR listening on socket");

    if ((new_socket = accept(sockfd, (struct sockaddr *)&server_addr, (socklen_t *)&addrlen)) < 0)
        tcp_error("ERROR accepting on socket");

    CHECK_RC(snk_create_field(15, 10, SNK_ARRAY_LEN(obstacles), obstacles, (uint32_t)time(NULL),
                              &field));

    CHECK_RC(snk_create(&field, &start_position, SNK_DIRECTION_RIGHT, 5, &process));

    if ((mutex = CreateMutexA(NULL, FALSE, NULL)) == NULL)
    {
        fprintf(stderr, "create mutex failed\n");
        _Exit(1);
    }
    data.mutex = mutex;
    data.proc = &process;

    if (CreateThread(NULL, 0, thread, &data, 0, NULL) == NULL)
    {
        fprintf(stderr, "create thread failed\n");
        _Exit(1);
    }


    while (1)
    {
        rc = recv( new_socket , &msg, sizeof(msg), 0);
        if (rc < 0 || msg.magic != DIRECTION_MSG_MAGIC)
            tcp_error("ERROR reading from socket");

        WaitForSingleObject(mutex, INFINITE);
        CHECK_RC(snk_choose_direction(&process, msg.direction));
        ReleaseMutex(mutex);
    }
}
