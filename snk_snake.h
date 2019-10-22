#ifndef __SNK_SNAKE_H__
#define __SNK_SNAKE_H__

#include "snk_defs.h"
#include "snk_joint.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Snake description */
typedef struct snk_snake {
    snk_position head_position; /**< Position of a snake's head */
    snk_direction head_direction; /**< Direction of a snake's head */
    snk_joint_buffer joints; /**< A snake's joints */
    uint32_t length; /**< Length of a snake */
    uint32_t pending_length; /**< Additional length which converts to actual length when a snake moves */
} snk_snake;

/**
 * Callback that is called in snk_snake_walk().
 *
 * @param[in] pos   Position of an element of snake's body
 * @param[in] data  Callback data
 *
 * @return          Status code
 */
typedef snk_rc_type (*snk_snake_walk_cb)(const snk_position *pos, void *data);

/**
 * Initialize a snake.
 *
 * @note Initialization parameters are described in snk_snake
 *
 * @param[out] snake    Snake to initialize
 */
void snk_snake_init(const snk_position *pos, snk_direction direction, const snk_joint_buffer *joints,
                    uint32_t length, uint32_t pending_length, snk_snake *snake);

/** Snake's head position getter */
const snk_position *snk_snake_get_head_position(const snk_snake *snake);

/**
 * Call a callback on every position of a snake's body.
 * If a callback returns an error, the functions returns that error immediately.
 *
 * @param[in] snake         Snake to iterate
 * @param[in] cb            Callback
 * @param[in,out] cb_data   Callback data
 *
 * @return                  Status code
 */
snk_rc_type snk_snake_walk(const snk_snake *snake, snk_snake_walk_cb cb, void *cb_data);

/**
 * Advance a snake's position by one unit.
 * - The head is moving towards @p next_direction;
 * - A new joint is created if the snake turns;
 * - Unused joints are deleted;
 * - Length of a snake is increased if pending_length is not 0;
 *
 * @note    The function does not check if the new head's position interferes with snake's body
 *
 * @param[in,out] snake         Snake to advance position of
 * @param[in] next_direction    Direction to advance snake towards
 *
 * @return                      Status code
 */
snk_rc_type snk_snake_advance(snk_snake *snake, snk_direction next_direction);

/**
 * Get all of the positions of a snake's head and body.
 *
 * @param[in] snake             Snake to get positions of
 * @param[in, out] n_positions  Size of the provided array, changes to number of retrieved position
 *                              if the function succeeds
 * @param[out] positions        Positions array
 *
 * @return                      Status code
 */
snk_rc_type snk_snake_get_positions(const snk_snake *snake, size_t *n_positions, snk_position *positions);

/**
 * Add pending length to a snake.
 *
 * @param[in,out] snake     Snake
 * @param length            Number that is added to pending length
 */
void snk_snake_add_pending_length(snk_snake *snake, uint32_t length);

/** Snake's length getter */
uint32_t snk_snake_get_length(const snk_snake *snake);

/** Snake's head direction getter */
snk_direction snk_snake_get_head_direction(const snk_snake *snake);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_SNAKE_H__ */
