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

static snk_rc_type
snk_check_snakes(size_t n_snakes, const snk_snake *snakes, const snk_field *field)
{
    snk_snake_position_iter iter;
    snk_position positions[64];
    size_t n_positions = 0;
    size_t snake_id;
    size_t i;

    for (snake_id = 0; snake_id < n_snakes; snake_id++)
    {
        SNK_SNAKE_FOREACH(&iter, &snakes[snake_id])
        {
            if (!snk_is_position_available(&iter.pos, field))
                return SNK_RC_INVALID;

            for (i = 0; i < n_positions; i++)
            {
                if (snk_position_equal(&iter.pos, &positions[i]))
                    return SNK_RC_INVALID;
            }

            if (n_positions >= SNK_ARRAY_LEN(positions))
                return SNK_RC_NOBUF;

            positions[n_positions++] = iter.pos;
        }
    }

    return 0;
}

snk_rc_type
snk_create(const snk_field *field, size_t n_snakes, const snk_position *start_position,
           const snk_direction *start_directions, const uint32_t *start_lengths, snk_process *process)
{
    snk_process result;
    snk_rc_type rc;
    size_t i;

    if (n_snakes > SNK_ARRAY_LEN(result.snakes))
        return SNK_RC_INVALID;

    for (i = 0; i < n_snakes; i++)
    {
        if (start_lengths[i] == 0)
            return SNK_RC_INVALID;
    }

    result.field = *field;

    for (i = 0; i < n_snakes; i++)
    {
        snk_snake_init(&start_position[i], start_directions[i], NULL, start_lengths[i], 0, &result.snakes[i]);
        result.next_directions[i] = start_directions[i];
    }

    rc = snk_check_snakes(n_snakes, result.snakes, &result.field);
    if (rc != 0)
        return rc;

    result.n_snakes = n_snakes;
    result.state = SNK_STATE_RUNNING;

    *process = result;

    return 0;
}

static snk_rc_type
snk_snakes_advance_in_field(size_t n_snakes, snk_snake *snakes, const snk_direction *next_directions, snk_field *field)
{
    snk_snake snakes_copy[SNK_SNAKES_MAX];
    snk_rc_type rc;
    size_t i;


    for (i = 0; i < n_snakes; i++)
    {
        snakes_copy[i] = snakes[i];
        rc = snk_snake_advance(&snakes_copy[i], next_directions[i]);
        if (rc != 0)
            return rc;
    }

    rc = snk_check_snakes(n_snakes, snakes_copy, field);
    if (rc != 0)
        return (rc == SNK_RC_INVALID ? SNK_RC_OVER : rc);

    for (i = 0; i < n_snakes; i++)
    {
        if (field->n_food > 0 &&
            snk_position_equal(snk_snake_get_head_position(&snakes_copy[i]), &field->food))
        {
            snk_snake_add_pending_length(&snakes_copy[i], 1);
            field->n_food = 0;
        }
    }

    for (i = 0; i < n_snakes; i++)
        snakes[i] = snakes_copy[i];

    return 0;
}

static snk_rc_type
snk_generate_food(const snk_snake *snake, snk_field *field)
{
    snk_position pos;
    snk_position snk_positions[64];
    size_t n_snk_positions = SNK_ARRAY_LEN(snk_positions);
    size_t i;
    snk_rc_type rc;

    if (field->n_food > 0)
        return 0;

    pos.x = snk_rand(&field->rand_seed) % field->width;
    pos.y = snk_rand(&field->rand_seed) % field->height;

    if (!snk_is_position_available(&pos, field))
        return 0;

    rc = snk_snake_get_positions(snake, &n_snk_positions, snk_positions);
    if (rc != 0)
        return rc;

    for (i = 0; i < n_snk_positions; i++)
    {
        if (snk_position_equal(&pos, &snk_positions[i]))
            return 0;
    }

    if (field->n_food > 0 && (snk_position_equal(&pos, &field->food)))
        return 0;

    field->n_food = 1;
    field->food = pos;

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

    rc = snk_generate_food(&process->snakes[0], &process->field);
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

    if (snake_id > process->n_snakes)
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
snk_render(const snk_process *process, uint8_t *data, size_t data_size)
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

    return 0;
}

snk_rc_type snk_get_score(snk_process *process, snk_score *score)
{
    uint32_t i;

    for (i = 0; i < process->n_snakes; i++)
        score->player[i] = snk_snake_get_length(&process->snakes[i]);

    return 0;
}
