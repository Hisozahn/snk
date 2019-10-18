#include <stdio.h>

#include "snk_snake.h"
#include "snk.h"

int
snk_snake_walk(const snk_snake *snake, snk_snake_walk_cb cb, void *cb_data)
{
    snk_direction direction = snk_direction_reverse(snake->head_direction);
    snk_position pos = snake->head_position;
    snk_joint next_joint;
    uint8_t joint_i;
    uint16_t i;
    int rc;

    for (i = 0, joint_i = 0; i < snake->length; i++)
    {
        printf("n_joints: %u\n", snake->joints.n_joints);
        if (snake->joints.n_joints > 0)
        {
            rc = snk_joint_get(&snake->joints, joint_i, &next_joint);
            if (rc == 0)
            {
                if (snk_position_compare(&pos, &next_joint.position) == 0)
                {
                    direction = next_joint.direction;
                    joint_i++;
                }
            }
            else if (rc != ENOENT)
                return rc;
        }

        rc = cb(&pos, cb_data);
        if (rc != 0)
            return rc;

        printf("%s: pos x: %u, y: %u, len: %u\n", __FUNCTION__, pos.x, pos.y, snake->length);
        snk_position_advance(&pos, direction);
    }

    return 0;
}

int
snk_snake_advance(snk_snake *snake, snk_direction next_direction)
{
    snk_joint joint = {snake->head_position, snk_direction_reverse(snake->head_direction)};
    int rc;

    if (snake->length > 2 &&
        snk_direction_reverse(snake->head_direction) == next_direction)
    {
        return EPERM;
    }

    if (snake->head_direction != next_direction)
    {
        rc = snk_joint_add(&snake->joints, &joint);
        if (rc != 0)
            return rc;
    }

    snake->head_direction = next_direction;
    snk_position_advance(&snake->head_position, snake->head_direction);
    if (snake->pending_length > 0)
    {
        snake->length++;
        snake->pending_length--;
    }

    return 0;
}