#include <stdio.h>
#include <windows.h>
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

    (void)argc;
    (void)argv;

    CHECK_RC(snk_create_field(15, 10, SNK_ARRAY_LEN(obstacles), obstacles, &field));

    CHECK_RC(snk_create(&field, &start_position, SNK_RIGHT, 4, &process));

    CHECK_RC(snk_start(&process));

    for (i = 0; i < 10; i++)
    {
        CHECK_RC(snk_next_tick(&process));

        CHECK_RC(snk_render(&process, draw_data, sizeof(draw_data)));

        draw(draw_data, process.field.width, process.field.height);
        Sleep(300);
        if (i == 4)
        {
            CHECK_RC(snk_choose_direction(&process, SNK_UP));
        }
    }

    return 0;
}
