#include "snk.h"
#include "snk_util.h"

static int
snk_is_position_possible(const snk_position *position, uint32_t field_width, uint32_t field_height)
{
    if (position->x >= field_width)
        return 0;

    if (position->y >= field_height)
        return 0;

    return 1;
}

static int
snk_position_within_obstacle(const snk_position *position, const snk_field_obstacle *obstacle)
{
    return (position->x >= obstacle->top_left.x &&
            position->y >= obstacle->top_left.y &&
            position->x <= obstacle->bottom_right.x &&
            position->y <= obstacle->bottom_right.y);
}

static int
snk_is_position_available(const snk_position *position, const snk_field *field)
{
    uint32_t i;

    if (!snk_is_position_possible(position, field->width, field->height))
        return 0;

    for (i = 0; i < field->n_obstacles; i++)
    {
        if (snk_position_within_obstacle(position, &field->obstacles[i]))
            return 0;
    }

    return 1;
}

static int
snk_is_field_obstacle_possible(const snk_field_obstacle *obstacle, uint32_t field_width, uint32_t field_height)
{
    if (snk_is_position_possible(&obstacle->top_left, field_width, field_height))
        return snk_is_position_possible(&obstacle->bottom_right, field_width, field_height);

    return 0;
}

snk_rc_type
snk_create_field(uint32_t width, uint32_t height, uint32_t n_obstacles, const snk_field_obstacle *obstacles,
                 uint32_t rand_seed, snk_field *field)
{
    size_t i;

    if (n_obstacles > SNK_ARRAY_LEN(field->obstacles))
        return SNK_RC_NOBUF;

    for (i = 0; i < n_obstacles; i++)
    {
        if (!snk_is_field_obstacle_possible(&obstacles[i], width, height))
            return SNK_RC_INVALID;
    }

    for (i = 0; i < n_obstacles; i++)
        field->obstacles[i] = obstacles[i];

    field->height = height;
    field->width = width;
    field->n_obstacles = n_obstacles;
    field->n_food = 0;
    field->rand_seed = rand_seed;

    return 0;
}

static int
snk_heads_cross(const snk_snake *snake_a, const snk_snake *snake_b)
{
    /* Brute force implementation */
    snk_position pos_a;
    snk_position pos_b;
    uint32_t id_a;
    uint32_t id_b;

    for (id_a = 0, pos_a = *snk_snake_get_head_position(snake_a); id_a < snake_a->head_length;
         snk_position_advance(&pos_a, snk_snake_get_head_direction(snake_a)), id_a++)
    {
        for (id_b = 0, pos_b = *snk_snake_get_head_position(snake_b); id_b < snake_b->head_length;
             snk_position_advance(&pos_b, snk_snake_get_head_direction(snake_b)), id_b++)
        {
            if (snk_position_equal(&pos_a, &pos_b))
                return 1;
        }
    }

    return 0;
}

static snk_rc_type
snk_check_snakes(size_t n_snakes, const snk_snake *snakes, const snk_field *field)
{
    snk_snake_position_iter iter;
    size_t i;
    size_t j;

    for (i = 0; i < n_snakes; i++)
    {
        SNK_SNAKE_FOREACH(&iter, &snakes[i])
        {
            if (!snk_is_position_available(&iter.pos, field))
                return SNK_RC_INVALID;
        }
    }

    for (i = 1; i < n_snakes; i++)
    {
        for (j = 0; j < i; j++)
        {
            if (snk_heads_cross(&snakes[i], &snakes[j]))
                return SNK_RC_INVALID;
        }
    }

    return 0;
}

snk_rc_type
snk_create(const snk_field *field, size_t n_snakes, const snk_position *start_position,
           const snk_direction *start_directions, const uint32_t *start_lengths, snk_process *process)
{
    snk_rc_type rc;
    size_t i;

    if (n_snakes > SNK_ARRAY_LEN(process->snakes))
        return SNK_RC_INVALID;

    for (i = 0; i < n_snakes; i++)
    {
        if (start_lengths[i] == 0)
            return SNK_RC_INVALID;
    }

    process->field = *field;

    for (i = 0; i < n_snakes; i++)
    {
        snk_snake_init(&start_position[i], start_directions[i], start_lengths[i], 0, &process->snakes[i]);
        process->next_directions[i] = start_directions[i];
    }

    rc = snk_check_snakes(n_snakes, process->snakes, &process->field);
    if (rc != 0)
        return rc;

    process->n_snakes = n_snakes;
    process->state = SNK_STATE_RUNNING;

    return 0;
}

/** Position check result */
enum snk_snake_position_check {
    SNK_SNAKE_POSITION_CHECK_TAKEN, /**< Position is taken */
    SNK_SNAKE_POSITION_CHECK_FREE, /**< Position is free */
    SNK_SNAKE_POSITION_CHECK_ON_TAIL, /**< Position is taken by a snake's tail */
};

static enum snk_snake_position_check
snk_is_position_valid_for_snake(size_t n_snakes, const snk_snake *snakes,
                                snk_field *field, const snk_position *pos,
                                size_t *collision_snake_id)
{
    snk_snake_position_iter iter;
    size_t i;

    if (collision_snake_id != NULL)
        *collision_snake_id = SIZE_MAX;

    if (!snk_is_position_available(pos, field))
        return SNK_SNAKE_POSITION_CHECK_TAKEN;

    for (i = 0; i < n_snakes; i++)
    {
        SNK_SNAKE_FOREACH(&iter, &snakes[i])
        {
            if (snk_position_equal(&iter.pos, pos))
            {
                if (collision_snake_id != NULL)
                    *collision_snake_id = i;

                snk_snake_pos_iter_next(&iter);

                return snk_snake_pos_iter_is_not_end(&iter) ? SNK_SNAKE_POSITION_CHECK_TAKEN :
                       SNK_SNAKE_POSITION_CHECK_ON_TAIL;
            }
        }
    }

    return SNK_SNAKE_POSITION_CHECK_FREE;
}

static snk_rc_type
snk_snakes_advance_in_field(size_t n_snakes, snk_snake *snakes, const snk_direction *next_directions, snk_field *field)
{
    snk_position heads_next[SNK_SNAKES_MAX];
    snk_rc_type rc;
    size_t i;
    size_t j;

    /* Get all future positions of the snakes' heads */
    for (i = 0; i < n_snakes; i++)
    {
        heads_next[i] = *snk_snake_get_head_position(&snakes[i]);
        snk_position_advance(&heads_next[i], next_directions[i]);
    }

    /* Check that the future heads positions do not interfere with anything */
    for (i = 0; i < n_snakes; i++)
    {
        size_t collision_snake_id;
        enum snk_snake_position_check check;

        check = snk_is_position_valid_for_snake(n_snakes, snakes, field, &heads_next[i], &collision_snake_id);
        switch (check)
        {
            case SNK_SNAKE_POSITION_CHECK_TAKEN:
                return SNK_RC_OVER;
                break;

            case SNK_SNAKE_POSITION_CHECK_ON_TAIL:
                if (snk_snake_get_pending_length(&snakes[collision_snake_id]) > 0)
                    return SNK_RC_OVER;

                if (field->n_food > 0 &&
                    snk_position_equal(&heads_next[collision_snake_id], &field->food))
                {
                    return SNK_RC_OVER;
                }
                break;

            default:
                break;
        }
    }

    /* Check that no two heads share the same position */
    for (i = 1; i < n_snakes; i++)
    {
        for (j = 0; j < i; j++)
        {
            if (snk_position_equal(&heads_next[i], &heads_next[j]))
                return SNK_RC_OVER;
        }
    }

    /* Check that advance for every snake will succeed */
    for (i = 0; i < n_snakes; i++)
    {
        rc = snk_snake_advance_check(&snakes[i], next_directions[i]);
        if (rc != 0)
            return rc;
    }

    /* Advance the snakes on the field */
    for (i = 0; i < n_snakes; i++)
        snk_snake_advance(&snakes[i], next_directions[i]);

    /* Perform eating the food by the snakes */
    for (i = 0; i < n_snakes; i++)
    {
        if (field->n_food > 0 &&
            snk_position_equal(snk_snake_get_head_position(&snakes[i]), &field->food))
        {
            snk_snake_add_pending_length(&snakes[i], 1);
            field->n_food = 0;
        }
    }

    return 0;
}

static int
snk_is_position_empty(size_t n_snakes, const snk_snake *snakes, snk_field *field, const snk_position *pos)
{
    if (snk_is_position_valid_for_snake(n_snakes, snakes, field, pos, NULL) != SNK_SNAKE_POSITION_CHECK_FREE)
        return 0;

    if (field->n_food > 0 && (snk_position_equal(pos, &field->food)))
        return 0;

    return 1;
}

static void
snk_place_food(snk_field *field, const snk_position *pos)
{
    field->n_food = 1;
    field->food = *pos;
}

static snk_rc_type
snk_check_and_place_food(size_t n_snakes, const snk_snake *snakes, snk_field *field, const snk_position *pos)
{
    if (snk_is_position_empty(n_snakes, snakes, field, pos))
    {
        snk_place_food(field, pos);
        return 0;
    }

    return SNK_RC_INVALID;
}

static snk_rc_type
snk_generate_food(size_t n_snakes, const snk_snake *snakes, snk_field *field)
{
    snk_direction order[2][2] = {{SNK_DIRECTION_LEFT, SNK_DIRECTION_UP}, {SNK_DIRECTION_RIGHT, SNK_DIRECTION_DOWN}};
    snk_position food_pos;
    uint32_t advance_len;
    size_t order_id;
    size_t half_id;
    size_t i;

    if (field->n_food > 0)
        return 0;

    food_pos.x = snk_rand(&field->rand_seed) % field->width;
    food_pos.y = snk_rand(&field->rand_seed) % field->height;

    if (snk_check_and_place_food(n_snakes, snakes, field, &food_pos) == 0)
        return 0;

    advance_len = 1;
    while (1)
    {
        for (order_id = 0; order_id < SNK_ARRAY_LEN(order); order_id++)
        {
            for (half_id = 0; half_id < SNK_ARRAY_LEN(order[0]); half_id++)
            {
                for (i = 0; i < advance_len; i++)
                {
                    snk_position_advance(&food_pos, order[order_id][half_id]);
                    if (snk_check_and_place_food(n_snakes, snakes, field, &food_pos) == 0)
                        return 0;
                }
            }

            advance_len++;
        }

        if (advance_len > field->width && advance_len > field->height)
            return SNK_RC_INVALID;
    }

    return 0;
}

snk_rc_type
snk_next_tick(snk_process *process)
{
    snk_rc_type rc;

    switch (process->state)
    {
        case SNK_STATE_RUNNING:
            break;
        case SNK_STATE_OVER:
            return SNK_RC_OVER;
        default:
            return SNK_RC_INVALID;
    }

    rc = snk_snakes_advance_in_field(process->n_snakes, process->snakes, process->next_directions, &process->field);
    if (rc != 0)
    {
        if (rc == SNK_RC_OVER)
            process->state = SNK_STATE_OVER;

        return rc;
    }

    rc = snk_generate_food(process->n_snakes, process->snakes, &process->field);
    if (rc != 0)
        return rc;

    return 0;
}

snk_rc_type
snk_choose_direction(snk_process *process, size_t snake_id, snk_direction direction)
{
    switch (process->state)
    {
        case SNK_STATE_RUNNING:
            break;
        case SNK_STATE_OVER:
            return SNK_RC_OVER;
        default:
            return SNK_RC_INVALID;
    }

    if (!snk_direction_is_valid(direction))
        return SNK_RC_INVALID;

    if (snake_id >= process->n_snakes)
        return SNK_RC_INVALID;

    /* Ignore direction if it is opposite to current direction of snake's head */
    if (direction != snk_direction_reverse(snk_snake_get_head_direction(&process->snakes[snake_id])))
        process->next_directions[snake_id] = direction;

    return 0;
}

static snk_rc_type
snk_render_position(const snk_field *field, const snk_position *pos,
                    snk_position_type type, uint8_t *data, size_t data_size)
{
    size_t index = (pos->y * field->width) + pos->x;

    if (index >= data_size)
        return SNK_RC_NOBUF;

    data[index] = type;

    return 0;
}

static snk_rc_type
snk_render_field_food(const snk_field *field, uint8_t *data, size_t data_size)
{
    snk_rc_type rc;

    if (field->n_food > 0)
    {
        rc = snk_render_position(field, &field->food, SNK_POSITION_FOOD, data, data_size);
        if (rc != 0)
            return rc;
    }

    return 0;
}

static snk_rc_type
snk_render_field_obstacle(const snk_field *field, const snk_field_obstacle *obstacle, uint8_t *data, size_t data_size)
{
    snk_position pos = obstacle->top_left;
    snk_position row_start;
    snk_rc_type rc;

    while (obstacle->bottom_right.y >= pos.y)
    {
        row_start = pos;

        while (obstacle->bottom_right.x >= pos.x)
        {
            rc = snk_render_position(field, &pos, SNK_POSITION_OBSTACLE, data, data_size);
            if (rc != 0)
                return rc;

            snk_position_advance(&pos, SNK_DIRECTION_RIGHT);
        }

        pos = row_start;
        snk_position_advance(&pos, SNK_DIRECTION_DOWN);
    }

    return 0;
}

snk_rc_type
snk_render(const snk_process *process, uint8_t *data, size_t data_size,
           uint32_t *width, uint32_t *height)
{
    snk_snake_position_iter iter;
    snk_rc_type rc;
    size_t i;

    if ((process->field.height * process->field.width) > data_size)
        return SNK_RC_NOBUF;

    for (i = 0; i < data_size; i++)
        data[i] = SNK_POSITION_EMPTY;

    for (i = 0; i < process->field.n_obstacles; i++)
    {
        rc = snk_render_field_obstacle(&process->field, &process->field.obstacles[i], data, data_size);
        if (rc != 0)
            return rc;
    }

    for (i = 0; i < process->field.n_food; i++)
    {
        rc = snk_render_field_food(&process->field, data, data_size);
        if (rc != 0)
            return rc;
    }

    for (i = 0; i < process->n_snakes; i++)
    {
        SNK_SNAKE_FOREACH(&iter, &process->snakes[i])
        {
            rc = snk_render_position(&process->field, &iter.pos, SNK_POSITION_SNAKE, data, data_size);
            if (rc != 0)
                return rc;
        }
    }

    *width = process->field.width;
    *height = process->field.height;

    return 0;
}

snk_rc_type snk_get_score(snk_process *process, snk_score *score)
{
    uint32_t i;

    for (i = 0; i < process->n_snakes; i++)
        score->player[i] = snk_snake_get_length(&process->snakes[i]);

    return 0;
}
