#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include "snk.h"
#include "snk_util.h"
#include "terminal.h"

/*
 * Valid from 1 to 2.
 * TODO: Some of the concurrent keyboard events are lost.
 */
#define PLAYERS_NUMBER 1

#define CHECK_RC(_call)                                                     \
do                                                                          \
{                                                                           \
    int __rc;                                                               \
    if ((__rc = (_call)) != 0)                                              \
    {                                                                       \
        printf("line %d, failed call %s: %d\n", __LINE__, #_call, __rc);    \
        goto cleanup;                                                       \
    }                                                                       \
} while (0)

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

int
main(int argc, char *argv[])
{
    /* Field setup */
    const uint32_t field_width = 80;
    const uint32_t field_height = 15;
    snk_field_obstacle obstacles[] = {{{0, 0}, {5, 0}}};

    /* Snakes setup */
    snk_position start_positions[] = {{5, 5}, {5, 7}};
    snk_direction start_directions[] = {SNK_DIRECTION_RIGHT, SNK_DIRECTION_RIGHT};
    uint32_t start_lengths[] = {5, 4};

    /* State machine and auxiliary structures */
    const uint32_t draw_data_size = field_width * field_height;
    uint8_t *draw_data = malloc(draw_data_size);
    terminal_data_t *td = NULL;
    snk_process process;
    snk_field field;
    int input;

    TERM_UNUSED(argc);
    TERM_UNUSED(argv);

    if (draw_data == NULL)
        CHECK_RC(ENOMEM);

    CHECK_RC(terminal_init(&td));

    CHECK_RC(snk_create_field(field_width, field_height, SNK_ARRAY_LEN(obstacles), obstacles,
                              (uint32_t)time(NULL), &field));

    CHECK_RC(snk_create(&field, PLAYERS_NUMBER, start_positions, start_directions, start_lengths, &process));

    while (1)
    {
        snk_score score;

        CHECK_RC(snk_render(&process, draw_data, draw_data_size));
        CHECK_RC(draw_data_convert(draw_data, draw_data_size));

        snk_get_score(&process, &score);
        CHECK_RC(terminal_draw(td, (char *)draw_data, process.field.width, process.field.height,
                               PLAYERS_NUMBER, &score));

        terminal_msleep(td, 100);

        if (terminal_get_char(td, &input) == 0)
        {
            snk_direction new_direction;
            size_t id;
            char c = (char)tolower(input);

            switch (c)
            {
                case 'a':
                    new_direction = SNK_DIRECTION_LEFT;
                    id = 0;
                    break;
                case 'd':
                    new_direction = SNK_DIRECTION_RIGHT;
                    id = 0;
                    break;
                case 'w':
                    new_direction = SNK_DIRECTION_UP;
                    id = 0;
                    break;
                case 's':
                    new_direction = SNK_DIRECTION_DOWN;
                    id = 0;
                    break;
                case 'j':
                    new_direction = SNK_DIRECTION_LEFT;
                    id = 1;
                    break;
                case 'l':
                    new_direction = SNK_DIRECTION_RIGHT;
                    id = 1;
                    break;
                case 'i':
                    new_direction = SNK_DIRECTION_UP;
                    id = 1;
                    break;
                case 'k':
                    new_direction = SNK_DIRECTION_DOWN;
                    id = 1;
                    break;
                default:
                    continue;
            }
            CHECK_RC(snk_choose_direction(&process, id, new_direction));
        }

        CHECK_RC(snk_next_tick(&process));
    }

cleanup:
    terminal_fini(td);
    free(draw_data);

    return 0;
}