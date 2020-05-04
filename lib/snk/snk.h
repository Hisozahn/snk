#ifndef __SNK_H__
#define __SNK_H__

#include "snk_defs.h"
#include "snk_snake.h"
#include "snk_joint.h"
#include "snk_position.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SNK_FIELD_OBSTACLE_MAX 32
#define SNK_SNAKES_MAX 4

/** Rectangular field obstacle which restricts snake's movement */
typedef struct snk_field_obstacle {
    snk_position top_left; /**< Top left corner of an obstacle */
    snk_position bottom_right; /**< Bottom right corner of an obstacle */
} snk_field_obstacle;

/** Field on which snake is moving */
typedef struct snk_field {
    uint32_t width; /**< Field's width */
    uint32_t height; /**< Field's height */
    snk_field_obstacle obstacles[SNK_FIELD_OBSTACLE_MAX]; /**< Field's obstacle collection */
    uint32_t n_obstacles; /**< Number of obstacles */
    snk_position food; /**< Position of a food item on the field */
    uint32_t n_food; /**< Number of food items on the field (only 0 and 1 are supported) */
    uint32_t rand_seed; /**< Random seed to generate location of food items */
} snk_field;

/** Snake process' state */
typedef enum snk_state {
    SNK_STATE_RUNNING, /**< Process is running well */
    SNK_STATE_OVER, /**< Process is stopped because the game is over */
} snk_state;

/** Process describing the current state of a snake game */
typedef struct snk_process {
    snk_field field; /**< Field */
    uint32_t n_snakes; /**< Number of snakes on the field */
    snk_snake snakes[SNK_SNAKES_MAX]; /**< Snakes on the field */
    snk_direction next_directions[SNK_SNAKES_MAX]; /**< Directions for the snakes to turn on the next tick */
    snk_state state; /**< Process' state */
} snk_process;

/** Type of a field position */
typedef enum snk_position_type {
    SNK_POSITION_EMPTY, /**< There is nothing at the position */
    SNK_POSITION_OBSTACLE, /**< There is an obstacle at the position */
    SNK_POSITION_SNAKE, /**< Snake body or head is at the position */
    SNK_POSITION_FOOD, /**< Food item is at the position */
} snk_position_type;

/** Type of a score */
typedef struct snk_score {
    uint32_t player[SNK_SNAKES_MAX]; /**< Players' scores, each value correponds to a snake
                                          with an ID equal to the index in the array */
} snk_score;

/**
 * Initialize a field. The consistency of the field's obstacles is checked
 *
 * @note Initialization parameters are described in snk_field
 *
 * @param[out] field    Field to initialize
 *
 * @return Status code
 */
snk_rc_type snk_create_field(uint32_t width, uint32_t height, uint32_t n_obstacles, const snk_field_obstacle *obstacles,
                     uint32_t rand_seed,  snk_field *field);

 /**
  * Initialize a snake process. The consistency of the snake position is checked.
  *
  * @note   After creation the numbers less than @p n_snakes are the snake IDs and represent
  *         a specific snake in a @p process.
  *
  * @param[in] field                Field
  * @param[in] n_snakes             Number of snakes on the field
  * @param[in] start_positions      Snakes' starting head positions
  * @param[in] start_directions     Snakes' starting head directions
  * @param[in] start_lengths        Snakes' starting lengths
  * @param[out] process             Snake process to initialize
  *
  * @return                         Status code
  */
snk_rc_type snk_create(const snk_field *field, size_t n_snakes, const snk_position *start_positions,
               const snk_direction *start_directions, const uint32_t *start_lengths, snk_process *process);

/**
 * Continue snake process for a step. Should be called once in a fixed time interval.
 *
 * @param[in,out] process   Snake process to continue
 * @return                  Status code
 * @retval SNK_RC_OVER      Game over condition
 */
snk_rc_type snk_next_tick(snk_process *process);

/**
 * Choose the next direction of a snake's movement.
 *
 * @note The movement is committed only on snk_next_tick.
 *       On 180 degrees turn the function succeeds,
 *       but the turn is ignored.
 *
 * @param[in,out] process   Snake process
 * @param[in] snake_id      ID of the snake to choose direction for
 * @param[in] direction     New direction
 *
 * @return                  Status code
 * @retval SNK_RC_INVALID   New direction is invalid
 */
snk_rc_type snk_choose_direction(snk_process *process, size_t snake_id, snk_direction direction);

/**
 * Get current score of a snake process.
 *
 * @param[in] process       Snake process
 * @param[out] score        Score
 *
 * @return                  Status code
 */
snk_rc_type snk_get_score(snk_process *process, snk_score *score);

/**
 * Render process' field with all objects on it.
 * Every element in @p data is filled with values from snk_position_type
 *
 * @param[in] process       Snake process
 * @param[out] data         Array to put rendered data
 * @param[in] data_size     Size of the provided array
 * @return                  Status code
 */
snk_rc_type snk_render(const snk_process *process, uint8_t *data, size_t data_size);
snk_rc_type snk_destroy(snk_process *process);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_H__ */
