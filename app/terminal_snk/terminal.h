#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "snk.h"

/* Mark a function argument as unused */
#define TERM_UNUSED(_arg) ((void)_arg)

/* Opaque terminal-specific structure */
typedef struct terminal_data_t terminal_data_t;

/* Setup terminal. td - location for allocated terminal data */
int terminal_init(terminal_data_t **td);

/* Finalize terminal and free the terminal data */
void terminal_fini(terminal_data_t *td);

/**
 * Get the last 'KEY DOWN' event from event queue.
 *
 * @param[in] td            Terminal data
 * @param[out] c            Contains character of the last 'KEY DOWN' event
 *                          on success
 *
 * @retval 1                No required events happened
 * @retval 0                Success
 */
int terminal_get_char(terminal_data_t *td, int *c);

/* Terminal or platform specific sleep */
void terminal_msleep(terminal_data_t *td, unsigned int milliseconds);

/**
 * Draw the game's object on the terminal
 *
 * @param td            Terminal data
 * @param draw_data     2D characters array to draw
 * @param width         Width of the 2D array
 * @param height        Height of the 2D array
 * @param n_players     Number of players in the game
 * @param score         The players' score
 *
 * @return              Status code
 */
int terminal_draw(terminal_data_t *td, const char *draw_data,
                  uint32_t width, uint32_t height,
                  unsigned int n_players, const snk_score *score);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __TERMINAL_H__ */
