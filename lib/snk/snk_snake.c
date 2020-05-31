#include "snk_snake.h"
#include "snk_util.h"

void
snk_snake_init(const snk_position *pos, snk_direction direction,
               uint32_t length, uint32_t pending_length,
               int8_t wrap_joints, snk_snake *snake)
{
    snk_joint_buffer_init(wrap_joints, &snake->joints);

    snake->head_direction = direction;
    snake->head_position = *pos;
    snake->head_length = length;
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
snk_snake_advance_check(const snk_snake *snake, snk_direction next_direction)
{
    if (snake->head_direction != next_direction)
        return snk_joint_buffer_add_check(&snake->joints);

    return 0;
}

void
snk_snake_advance(snk_snake *snake, snk_direction next_direction)
{
    if (snake->head_direction != next_direction)
    {
        snk_joint joint;

        snk_joint_init(&snake->head_position, snk_direction_reverse(snake->head_direction), snake->head_length, &joint);
        snk_joint_buffer_add(&snake->joints, &joint);
        snake->head_length = 1;
    }
    else if (snk_joint_buffer_size(&snake->joints) > 0 || snake->pending_length > 0)
    {
        snake->head_length++;
    }

    snake->head_direction = next_direction;
    snk_position_advance(&snake->head_position, snake->head_direction);

    if (snk_joint_buffer_size(&snake->joints) > 0 && snake->pending_length == 0)
    {
        snk_joint *last;

        last = snk_joint_buffer_get_mutable(&snake->joints, snk_joint_buffer_size(&snake->joints) - 1);
        last->length--;

        if (last->length == 0)
            snk_joint_buffer_del(&snake->joints);
    }

    if (snake->pending_length > 0)
        snake->pending_length--;
}

void
snk_snake_add_pending_length(snk_snake *snake, uint32_t length)
{
    snake->pending_length += length;
}

uint32_t
snk_snake_get_length(const snk_snake *snake)
{
    uint32_t result = 0;
    uint32_t i;

    for (i = 0 ; i < snk_joint_buffer_size(&snake->joints); i++)
        result += snk_joint_buffer_get(&snake->joints, i)->length;

    return result + snake->head_length;
}

uint32_t
snk_snake_get_pending_length(const snk_snake *snake)
{
    return snake->pending_length;
}

void
snk_snake_pos_iter_init(snk_snake_position_iter *iter, const snk_snake *snake)
{
    iter->snake = snake;
    iter->pos = snake->head_position;
    iter->len = snake->head_length;
    iter->joint_i = 0;
    iter->direction = snk_direction_reverse(snake->head_direction);
}

int
snk_snake_pos_iter_is_not_end(const snk_snake_position_iter *iter)
{
    return iter->len != 0;
}

void
snk_snake_pos_iter_next(snk_snake_position_iter *iter)
{
    const snk_joint *next_joint;

    snk_position_advance(&iter->pos, iter->direction);
    iter->len--;

    if (iter->len == 0 && snk_joint_buffer_size(&iter->snake->joints) > 0)
    {
        next_joint = snk_joint_buffer_get(&iter->snake->joints, iter->joint_i);
        if (next_joint != NULL)
        {
            iter->direction = snk_joint_get_direction(next_joint);
            iter->joint_i++;
            iter->len = next_joint->length;
        }
    }
}