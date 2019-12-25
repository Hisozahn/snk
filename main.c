#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <strsafe.h>
#include <errno.h>
#include <time.h>
#include <wincon.h>
#include "snk.h"
#include "snk_util.h"
#include "tcp_app.h"
#include "tcp_util.h"

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

void Error()
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

    fprintf(stderr, "%s\n", (const char *)lpMsgBuf);

    LocalFree(lpMsgBuf);
}

DWORD WINAPI client_thread(void *arg)
{
    tcp_client_main(arg);
}

DWORD WINAPI server_thread(void *arg)
{
    tcp_server_main(arg);
}

int
main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
    HANDLE server_handle;
    HANDLE client_handle;

    win_socket_init();

    if (argc < 3)
    {
        fprintf(stderr, "arguments are invalid\n");
        _Exit(1);
    }

    server_addr = parse_address(argv[1], atoi(argv[2]));

    if ((server_handle = CreateThread(NULL, 0, client_thread, &server_addr, 0, NULL)) == NULL)
    {
        fprintf(stderr, "create client thread failed\n");
        _Exit(1);
    }

    if ((client_handle = CreateThread(NULL, 0, server_thread, &server_addr, 0, NULL)) == NULL)
    {
        fprintf(stderr, "create client thread failed\n");
        _Exit(1);
    }
    WaitForSingleObject(server_handle, INFINITE);
    WaitForSingleObject(client_handle, INFINITE);

    return 0;
}
