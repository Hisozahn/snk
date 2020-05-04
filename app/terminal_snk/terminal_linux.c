#include <unistd.h>
#include <ncurses.h>
#include <errno.h>
#include <time.h>

#include "terminal.h"

static uint32_t
terminal_min(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}

int
terminal_init(terminal_data_t **td)
{
    TERM_UNUSED(td);

    if (initscr() == NULL)
        return EFAULT;

    if (noecho() == ERR)
        return EFAULT;

    /* Ignore errors here since it may not be supported */
    (void)curs_set(0);

    timeout(0);

    return 0;
}

void
terminal_fini(terminal_data_t *td)
{
    TERM_UNUSED(td);

    endwin();
}

int
terminal_get_char(terminal_data_t *td, int *c)
{
    int character;
    int rc = 1;

    TERM_UNUSED(td);

    while ((character = getch()) != ERR)
    {
        if (character != ERR)
        {
            *c = character;
            rc = 0;
        }
    }

    return rc;
}

int
terminal_draw(terminal_data_t *td, const char *draw_data, uint32_t width, uint32_t height,
              unsigned int n_players, const snk_score *score)
{
    uint32_t i;
    uint32_t max_height;
    uint32_t max_width;

    TERM_UNUSED(td);

    getmaxyx(stdscr, max_height, max_width);

    if (move(0,0) == ERR)
        return EFAULT;

    for (i = 0; max_height > 0 && i < n_players; i++)
    {
        (void)printw("%s %u", i == 0 ? "Score:" : "", score->player[i]);
    }

    for (i = 0; i < height && i < max_height - 1; i++)
    {
        (void)mvprintw(i + 1, 0, "%.*s", terminal_min(width, max_width), &draw_data[i * width]);
    }

    if (refresh() == ERR)
        return EFAULT;

    return 0;
}

void
terminal_msleep(terminal_data_t *td, unsigned int milliseconds)
{
    struct timespec spec = {milliseconds / 1000, (milliseconds % 1000) * 1000000};

    TERM_UNUSED(td);

    while (nanosleep(&spec, &spec) < 0 && errno == EINTR);
}