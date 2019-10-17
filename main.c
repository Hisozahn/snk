#include <stdio.h>
#include "snk.h"
#include "snk_util.h"


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
    int rc;
    uint8_t draw_data[2048];
    snk_field_obstacle obstacles[] = {{{0, 0}, {5, 0}}};
    snk_position start_position = {5, 5};
    snk_process process;
    snk_field field;

    (void)argc;
    (void)argv;

    rc = snk_create_field(15, 10, SNK_ARRAY_LEN(obstacles), obstacles, &field);
    printf("%d rc: %d\n", __LINE__, rc);
    if (rc == 0)
        rc = snk_create(&field, &start_position, SNK_RIGHT, 3, &process);
    printf("%d rc: %d\n", __LINE__, rc);

    if (rc == 0)
        rc = snk_start(&process);
    printf("%d rc: %d\n", __LINE__, rc);

    if (rc == 0)
        rc = snk_next_tick(&process);
    printf("%d rc: %d\n", __LINE__, rc);

    if (rc == 0)
        rc = snk_render(&process, draw_data, sizeof(draw_data));
    printf("%d rc: %d\n", __LINE__, rc);
    if (rc == 0)
        draw(draw_data, process.field.width, process.field.height);

    return 0;
}
