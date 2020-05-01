#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include "snk.h"
#include "snk_util.h"

#define PLAYERS_NUMBER 2

#define CHECK_RC(_call)                                             \
do                                                                  \
{                                                                   \
    if ((rc = (_call)) != 0)                                               \
    {                                                               \
        printf("line %d, failed call %s: %d\n", __LINE__, #_call, rc);      \
        exit(1);                                                   \
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

int
main(int argc, char *argv[])
{
    int i;
    int j;
    uint8_t draw_data[2048];
    snk_field_obstacle obstacles[PLAYERS_NUMBER] = {{{0, 0}, {5, 0}}};
    snk_position start_positions[PLAYERS_NUMBER] = {{5, 5}, {5, 7}};
    snk_direction start_directions[PLAYERS_NUMBER] = {SNK_DIRECTION_RIGHT, SNK_DIRECTION_RIGHT};
    uint32_t start_lengths[PLAYERS_NUMBER] = {5, 4};
    snk_process process;
    snk_field field;
    int rc;
    char input[1];

    (void)argc;
    (void)argv;

    CHECK_RC(snk_create_field(15, 10, SNK_ARRAY_LEN(obstacles), obstacles, (uint32_t)time(NULL),
            &field));

    CHECK_RC(snk_create(&field, PLAYERS_NUMBER, start_positions, start_directions, start_lengths, &process));

    for (i = 0; i < 100; i++)
    {
        snk_score score;

        CHECK_RC(snk_next_tick(&process));
        snk_get_score(&process, &score);

        printf("score:");
        for (j = 0; j < PLAYERS_NUMBER; j++)
            printf(" %u", score.player[j]);
        printf("\n");

        CHECK_RC(snk_render(&process, draw_data, sizeof(draw_data)));

        CHECK_RC(draw_data_convert(draw_data, sizeof(draw_data)));

        draw(draw_data, process.field.width, process.field.height);

        retry:

        rc = fread(input, sizeof(input), 1, stdin);
        if (rc != sizeof(input))
            break;

        snk_direction new_direction;
        size_t id;
        char c = (char)tolower(input[0]);

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
                goto retry;

        }
        CHECK_RC(snk_choose_direction(&process, id, new_direction));
    }

    return 0;
}
