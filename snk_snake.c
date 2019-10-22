#include "snk_snake.h"

void
snk_snake_init(const snk_position *pos, snk_direction direction, const snk_joint_buffer *joints,
               uint32_t length, uint32_t pending_length, snk_snake *snake)
{
    if (joints != NULL)
        snake->joints = *joints;
    else
        snk_joint_buffer_init(&snake->joints);

    snake->head_direction = direction;
    snake->head_position = *pos;
    snake->length = length;
    snake->pending_length = pending_length;
}

const snk_position *
snk_snake_get_head_position(const snk_snake *snake)
{
    return &snake->head_position;
}

snk_direction
snk_snake_get_head_direction(const snk_snake *snake)
{
    return snake->head_direction;
}

static snk_rc_type
snk_snake_walk_impl(const snk_snake *snake, snk_snake_walk_cb cb, void *cb_data, uint32_t *n_used_joints)
{
    snk_direction direction = snk_direction_reverse(snake->head_direction);
    snk_position pos = snake->head_position;
    snk_joint next_joint;
    uint32_t joint_i;
    uint32_t i;
    snk_rc_type rc;

    for (i = 0, joint_i = 0; i < snake->length; i++)
    {
        if (snk_joint_buffer_size(&snake->joints) > 0)
        {
            rc = snk_joint_buffer_get(&snake->joints, joint_i, &next_joint);
            if (rc == SNK_RC_SUCCESS)
            {
                if (snk_position_compare(&pos, snk_joint_get_position(&next_joint)) == 0)
                {
                    direction = snk_joint_get_direction(&next_joint);
                    joint_i++;
                }
            }
            else if (rc != SNK_RC_NOENT)
                return rc;
        }

        if (cb != NULL)
        {
            rc = cb(&pos, cb_data);
            if (rc != SNK_RC_SUCCESS)
                return rc;
        }

        snk_position_advance(&pos, direction);
    }

    if (n_used_joints != NULL)
        *n_used_joints = joint_i;

    return SNK_RC_SUCCESS;
}

snk_rc_type
snk_snake_walk(const snk_snake *snake, snk_snake_walk_cb cb, void *cb_data)
{
    return snk_snake_walk_impl(snake, cb, cb_data, NULL);
}

snk_rc_type
snk_snake_advance(snk_snake *snake, snk_direction next_direction)
{
    snk_snake snake_copy = *snake;
    snk_joint joint;
    uint32_t n_used_joints;
    uint32_t i;
    snk_rc_type rc;

    snk_joint_init(&snake_copy.head_position, snk_direction_reverse(snake_copy.head_direction), &joint);

    if (snake_copy.head_direction != next_direction)
    {
        rc = snk_joint_buffer_add(&snake_copy.joints, &joint);
        if (rc != SNK_RC_SUCCESS)
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
    if (rc != SNK_RC_SUCCESS)
        return rc;

    for (i = n_used_joints; i < snk_joint_buffer_size(&snake_copy.joints); i++)
    {
        rc = snk_joint_buffer_del(&snake_copy.joints);
        if (rc != SNK_RC_SUCCESS)
            return rc;
    }

    *snake = snake_copy;

    return SNK_RC_SUCCESS;
}

struct snk_snake_positions_data {
    snk_position *positions;
    size_t n_positions_in;
    size_t n_positions_out;
};

static snk_rc_type
snk_snake_get_positions_cb(const snk_position *pos, void *data)
{
    struct snk_snake_positions_data *pos_data = data;

    if (pos_data->n_positions_out >= pos_data->n_positions_in)
        return SNK_RC_NOBUF;

    pos_data->positions[pos_data->n_positions_out] = *pos;
    pos_data->n_positions_out++;

    return SNK_RC_SUCCESS;
}

snk_rc_type
snk_snake_get_positions(const snk_snake *snake, size_t *n_positions, snk_position *positions)
{
    struct snk_snake_positions_data data = {positions, *n_positions, 0};
    snk_rc_type rc;

    rc = snk_snake_walk(snake, snk_snake_get_positions_cb, &data);
    if (rc != SNK_RC_SUCCESS)
        return rc;

    *n_positions = data.n_positions_out;

    return rc;
}

void
snk_snake_add_pending_length(snk_snake *snake, uint32_t length)
{
    snake->pending_length += length;
}

uint32_t
snk_snake_get_length(const snk_snake *snake)
{
    return snake->length;
}