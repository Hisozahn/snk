#include <stdio.h>

#include "snk_snake.h"
#include "snk.h"

static int
snk_snake_walk_impl(const snk_snake *snake, snk_snake_walk_cb cb, void *cb_data, uint32_t *n_used_joints)
{
    snk_direction direction = snk_direction_reverse(snake->head_direction);
    snk_position pos = snake->head_position;
    snk_joint next_joint;
    uint32_t joint_i;
    uint32_t i;
    int rc;

    for (i = 0, joint_i = 0; i < snake->length; i++)
    {
        //printf("n_joints: %u\n", snake->joints.n_joints);
        if (snk_joint_size(&snake->joints) > 0)
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

        //printf("%s: CALLBACK pos x: %u, y: %u\n", __FUNCTION__, pos.x, pos.y);
        if (cb != NULL)
        {
            rc = cb(&pos, cb_data);
            if (rc != 0)
                return rc;
        }

        //printf("%s: pos x: %u, y: %u, len: %u\n", __FUNCTION__, pos.x, pos.y, snake->length);
        snk_position_advance(&pos, direction);
    }

    if (n_used_joints != NULL)
        *n_used_joints = joint_i;

    return 0;
}

int
snk_snake_walk(const snk_snake *snake, snk_snake_walk_cb cb, void *cb_data)
{
    return snk_snake_walk_impl(snake, cb, cb_data, NULL);
}

int
snk_snake_advance(snk_snake *snake, snk_direction next_direction)
{
    snk_snake snake_copy = *snake;
    snk_joint joint = {snake_copy.head_position, snk_direction_reverse(snake_copy.head_direction)};
    uint32_t n_used_joints;
    uint32_t i;
    int rc;

    if (snake_copy.length > 2 &&
        snk_direction_reverse(snake_copy.head_direction) == next_direction)
    {
        return EPERM;
    }

    if (snake_copy.head_direction != next_direction)
    {
        rc = snk_joint_add(&snake_copy.joints, &joint);
        if (rc != 0)
            return rc;
    }

    snake_copy.head_direction = next_direction;
    snk_position_advance(&snake_copy.head_position, snake_copy.head_direction);
    if (snake_copy.pending_length > 0)
    {
        snake_copy.length++;
        snake_copy.pending_length--;
    }

    rc = snk_snake_walk_impl(&snake_copy, NULL, NULL, &n_used_joints);
    if (rc != 0)
        return rc;

    for (i = n_used_joints; i < snk_joint_size(&snake_copy.joints); i++)
    {
        rc = snk_joint_del(&snake_copy.joints);
        if (rc != 0)
            return rc;
    }

    //printf("N_USED_JOINTS: %u\n", n_used_joints);
    *snake = snake_copy;

    return 0;
}

struct snk_snake_positions_data {
    snk_position *positions;
    size_t n_positions_in;
    size_t n_positions_out;
};

static int
snk_snake_get_positions_cb(const snk_position *pos, void *data)
{
    struct snk_snake_positions_data *pos_data = data;

    if (pos_data->n_positions_out >= pos_data->n_positions_in)
        return EPERM;

    pos_data->positions[pos_data->n_positions_out] = *pos;
    pos_data->n_positions_out++;

    return 0;
}

int
snk_snake_get_positions(const snk_snake *snake, size_t *n_positions, snk_position *positions)
{
    struct snk_snake_positions_data data = {positions, *n_positions, 0};
    int rc;

    rc = snk_snake_walk(snake, snk_snake_get_positions_cb, &data);
    if (rc != 0)
        return rc;

    *n_positions = data.n_positions_out;

    return rc;
}

void
snk_snake_add_pending_length(snk_snake *snake, uint32_t length)
{
    snake->pending_length += length;
}