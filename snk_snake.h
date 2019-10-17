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
    uint16_t length;
    uint16_t pending_length;
} snk_snake;

typedef int (*snk_snake_walk_cb)(const snk_position *pos, void *data);

int snk_snake_walk(const snk_snake *snake, snk_snake_walk_cb cb, void *cb_data);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_SNAKE_H__ */
