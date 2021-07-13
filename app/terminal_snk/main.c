#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include "snk.h"
#include "snk_util.h"
#include "terminal.h"
#include "snk_config.h"
#include "snk_factory.h"

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

static const struct snk_config default_snk_config = {
    .settings = {
        .wrap_joints = 1,
        .rand_seed = SNK_CONFIG_RAND_SEED_UNSET,
    },

    .field = {
        .width = 60,
        .height = 25,
        .n_obstacles = 0,
    },

    .snakes = {
        .start_positions = {{ .x = 5, .y = 5 }, { .x = 5, .y = 10 }},
        .start_directions = { SNK_DIRECTION_RIGHT, SNK_DIRECTION_RIGHT },
        .start_lengths = { 5, 5 },
        .n_snakes = 2,
    }
};

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
    /* State machine and auxiliary structures */
    uint32_t draw_data_size;
    uint8_t *draw_data = NULL;
    terminal_data_t *td = NULL;
    snk_process process;
    size_t n_players;
    int input;
    snk_config config = default_snk_config;

    if (argc > 1)
    {
        FILE *f;
        char buf[4096] = {0};
        size_t r;

        f = fopen(argv[1], "r");
        if (f == NULL)
        {
            fprintf(stderr, "%s: %d\n", __func__, __LINE__);
            CHECK_RC(EFAULT);
        }

        r = fread(buf, 1, sizeof(buf), f);
        if (r > 0 && r < sizeof(buf))
        {
            CHECK_RC(snk_config_load_from_json(buf, &config));
            fprintf(stderr, "%s: %d READ CONFIG\n", __func__, __LINE__);
        }
    }

    snk_config_set_seed_if_unset(&config, time(NULL));
    CHECK_RC(snk_create_from_config(&config, &process));

    n_players = snk_n_players(&process);
    draw_data_size = snk_render_data_size(&process);
    draw_data = malloc(draw_data_size);

    if (draw_data == NULL)
        CHECK_RC(ENOMEM);

    CHECK_RC(terminal_init(&td));

    while (1)
    {
        snk_score score;
        uint32_t render_width;
        uint32_t render_height;

        CHECK_RC(snk_render(&process, draw_data, draw_data_size, &render_width, &render_height));
        CHECK_RC(draw_data_convert(draw_data, render_width * render_height));

        snk_get_score(&process, &score);
        CHECK_RC(terminal_draw(td, (char *)draw_data, render_width, render_height,
                               n_players, &score));

        terminal_msleep(td, 100);

        if (terminal_get_char(td, &input) == 0)
        {
            snk_direction new_direction = SNK_DIRECTION_INVALID;
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
                    break;
            }
            if (new_direction != SNK_DIRECTION_INVALID)
                CHECK_RC(snk_choose_direction(&process, id, new_direction));
        }

        CHECK_RC(snk_next_tick(&process));
    }

cleanup:
    terminal_fini(td);
    free(draw_data);

    return 0;
}
