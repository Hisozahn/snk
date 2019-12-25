#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <errno.h>
#include <wincon.h>
#include "snk.h"
#include "snk_util.h"
#include "tcp_util.h"
#pragma comment(lib, "ws2_32.lib")

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
    int rc;

    win_socket_init();

    if (argc < 3)
    {
        fprintf(stderr, "arguments are invalid\n");
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