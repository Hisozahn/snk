#include "snk_snake.h"

int
snk_snake_walk(const snk_snake *snake, snk_snake_walk_cb cb, void *cb_data)
{
    snk_direction direction = snake->head_direction;
    snk_position pos = snake->head_position;
    snk_joint next_joint;
    uint8_t joint_i;
    uint16_t i;
    int rc;

    for (i = 0, joint_i = 0; i < snake->length; i++)
    {
        if (i == 0 ||
            snk_position_compare(&pos, &next_joint.position) == 0)
        {
            rc = snk_joint_get(&snake->joints, joint_i++, &next_joint);
            if (rc != 0)
                return rc;

            direction = next_joint.direction;
        }

        cb(&pos, cb_data);

        snk_position_advance(&pos, direction);
    }

    return 0;
}