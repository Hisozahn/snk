#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <time.h>
#include "snk.h"
#include "snk_util.h"

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
draw(const uint8_t *draw_data, uint8_t width, uint8_t height)
{
    uint8_t i;
    for (i = 0; i < height; i++)
    {
        printf("%.*s\n", width, &draw_data[i * width]);
    }
}

int
main(int argc, char *argv[])
{
    int i;
    uint8_t draw_data[2048];
    snk_field_obstacle obstacles[] = {{{0, 0}, {5, 0}}};
    snk_position start_position = {5, 5};
    snk_process process;
    snk_field field;
    int rc;
    int input;

    (void)argc;
    (void)argv;

    CHECK_RC(snk_create_field(15, 10, SNK_ARRAY_LEN(obstacles), obstacles, time(NULL),
            &field));

    CHECK_RC(snk_create(&field, &start_position, SNK_RIGHT, 5, &process));

    for (i = 0; i < 100; i++)
    {
        CHECK_RC(snk_next_tick(&process));

        CHECK_RC(snk_render(&process, draw_data, sizeof(draw_data)));

        draw(draw_data, process.field.width, process.field.height);
        //Sleep(1000);
        retry:
        if ((input = getchar()) != EOF)
        {
            snk_direction new_direction;
            char c = (char)tolower(input);

            switch (c)
            {
                case 'a':
                    new_direction = SNK_LEFT;
                    break;
                case 'd':
                    new_direction = SNK_RIGHT;
                    break;
                case 'w':
                    new_direction = SNK_UP;
                    break;
                case 's':
                    new_direction = SNK_DOWN;
                    break;
                default:
                    goto retry;

            }
            CHECK_RC(snk_choose_direction(&process, new_direction));
        }
    }

    return 0;
}
