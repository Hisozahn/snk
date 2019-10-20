#ifndef __SNK_SNAKE_H__
#define __SNK_SNAKE_H__

#include "snk_defs.h"
#include "snk_joint.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Snake description */
typedef struct snk_snake {
    snk_position head_position;
    snk_direction head_direction;
    snk_joint_buffer joints;
    uint32_t length;
    uint32_t pending_length;
} snk_snake;

typedef snk_rc_type (*snk_snake_walk_cb)(const snk_position *pos, void *data);

void snk_snake_init(const snk_position *pos, snk_direction direction, const snk_joint_buffer *joints,
                    uint32_t length, uint32_t pending_length, snk_snake *snake);
const snk_position *snk_snake_get_head_position(const snk_snake *snake);
snk_rc_type snk_snake_walk(const snk_snake *snake, snk_snake_walk_cb cb, void *cb_data);
snk_rc_type snk_snake_advance(snk_snake *snake, snk_direction next_direction);
snk_rc_type snk_snake_get_positions(const snk_snake *snake, size_t *n_positions, snk_position *positions);
void snk_snake_add_pending_length(snk_snake *snake, uint32_t length);
uint32_t snk_snake_get_length(const snk_snake *snake);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_SNAKE_H__ */
