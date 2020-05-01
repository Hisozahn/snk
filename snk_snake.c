#include "snk_snake.h"
#include "snk_util.h"

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

snk_rc_type
snk_snake_advance(snk_snake *snake, snk_direction next_direction)
{
    snk_snake snake_copy = *snake;
    snk_joint joint;
    uint32_t i;
    snk_snake_position_iter iter;
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

    SNK_SNAKE_FOREACH(&iter, &snake_copy);
    for (i = iter.joint_i; i < snk_joint_buffer_size(&snake_copy.joints); i++)
    {
        rc = snk_joint_buffer_del(&snake_copy.joints);
        if (rc != SNK_RC_SUCCESS)
            return rc;
    }

    *snake = snake_copy;

    return SNK_RC_SUCCESS;
}

snk_rc_type
snk_snake_get_positions(const snk_snake *snake, size_t *n_positions, snk_position *positions)
{
    snk_snake_position_iter iter;
    size_t size = 0;

    SNK_SNAKE_FOREACH(&iter, snake)
    {
        if (size >= *n_positions)
            return SNK_RC_NOBUF;

        positions[size] = iter.pos;
        size++;
    }

    *n_positions = size;

    return SNK_RC_SUCCESS;
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

void
snk_snake_pos_iter_init(snk_snake_position_iter *iter, const snk_snake *snake)
{
    iter->snake = snake;
    iter->pos = snake->head_position;
    iter->i = 0;
    iter->joint_i = 0;
    iter->direction = snk_direction_reverse(snake->head_direction);
}

int
snk_snake_pos_iter_has_next(const snk_snake_position_iter *iter)
{
    return iter->i < iter->snake->length;
}

void
snk_snake_pos_iter_next(snk_snake_position_iter *iter)
{
    const snk_joint *next_joint;

    iter->i++;

    if (snk_joint_buffer_size(&iter->snake->joints) > 0)
    {
        next_joint = snk_joint_buffer_get(&iter->snake->joints, iter->joint_i);
        if (next_joint != NULL)
        {
            if (snk_position_equal(&iter->pos, snk_joint_get_position(next_joint)))
            {
                iter->direction = snk_joint_get_direction(next_joint);
                iter->joint_i++;
            }
        }
    }

    snk_position_advance(&iter->pos, iter->direction);
}