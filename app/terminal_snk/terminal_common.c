#include <stdio.h>

#include "terminal.h"

int
terminal_init(terminal_data_t **td)
{
    TERM_UNUSED(td);

    return 0;
}

void
terminal_fini(terminal_data_t *td)
{
    TERM_UNUSED(td);
}

int
terminal_get_char(terminal_data_t *td, int *c)
{
    TERM_UNUSED(td);

    *c = getchar();

    return 0;
}

int
terminal_draw(terminal_data_t *td, const char *draw_data, uint32_t width, uint32_t height,
              unsigned int n_players, const snk_score *score)
{
    uint32_t i;

    TERM_UNUSED(td);

    for (i = 0; i < n_players; i++)
        (void)printf("%s %u", i == 0 ? "Score:" : "", score->player[i]);

    puts("");
    for (i = 0; i < height; i++)
        (void)printf("%.*s\n", width, &draw_data[i * width]);

    return 0;
}

void
terminal_msleep(terminal_data_t *td, unsigned int milliseconds)
{
}