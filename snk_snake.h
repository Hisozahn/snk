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

/** Snake body positions iterator */
typedef struct snk_snake_position_iter {
    const snk_snake *snake; /**< Snake to iterate */
    snk_position pos; /**< Current position */
    snk_direction direction; /**< Current direction of the iterator */
    uint32_t i; /**< Current index of the snake's body cell */
    uint32_t joint_i; /**< Current index of the snake's joint
                           Use this as the number of joints that were required to iterate
                           the whole snake after using SNK_SNAKE_FOREACH */
} snk_snake_position_iter;

/** Init snake body positions iterator. It starts from the snake's head. */
void snk_snake_pos_iter_init(snk_snake_position_iter *iter, const snk_snake *snake);

/** Snake body positions iterator's check that it is not finished */
int snk_snake_pos_iter_is_not_end(const snk_snake_position_iter *iter);

/** Move snake body positions iterator to the next position */
void snk_snake_pos_iter_next(snk_snake_position_iter *iter);

/**
 * Iterate snake body positions starting from the snake's head.
 *
 * @param _iter     Position iterator pointer (snk_snake_position_iter *)
 * @param _snake    Snake object (const snk_snake *)
 */
#define SNK_SNAKE_FOREACH(_iter, _snake) \
    for (snk_snake_pos_iter_init((_iter), (_snake)); \
         snk_snake_pos_iter_is_not_end(_iter); \
         snk_snake_pos_iter_next(_iter))

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

/** Check whether the next call to snk_snake_advance() will succeed. */
snk_rc_type snk_snake_advance_check(const snk_snake *snake, snk_direction next_direction);

/**
 * Advance a snake's position by one unit.
 * - The head is moving towards @p next_direction;
 * - A new joint is created if the snake turns;
 * - Unused joints are deleted;
 * - Length of a snake is increased if pending_length is not 0;
 *
 * @note    The function does not check if the new head's position interferes with snake's body
 * @note    Use snk_snake_advance_check() to check whether this operation will succeed.
 *
 * @param[in,out] snake         Snake to advance position of
 * @param[in] next_direction    Direction to advance snake towards
 */
void snk_snake_advance(snk_snake *snake, snk_direction next_direction);

/**
 * Add pending length to a snake.
 *
 * @param[in,out] snake     Snake
 * @param length            Number that is added to pending length
 */
void snk_snake_add_pending_length(snk_snake *snake, uint32_t length);

/** Snake's length getter */
uint32_t snk_snake_get_length(const snk_snake *snake);

/** Snake's pending length getter */
uint32_t snk_snake_get_pending_length(const snk_snake *snake);

/** Snake's head direction getter */
snk_direction snk_snake_get_head_direction(const snk_snake *snake);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_SNAKE_H__ */
