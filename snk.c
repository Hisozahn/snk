#include "snk.h"
#include "snk_util.h"

static snk_rc_type
snk_check_position_possible(const snk_position *position, uint32_t field_width, uint32_t field_height)
{
    if (position->x < 0 || position->x >= field_width)
        return SNK_RC_INVALID;

    if (position->y < 0 || position->y >= field_height)
        return SNK_RC_INVALID;

    return SNK_RC_SUCCESS;
}

static int
snk_position_within_obstacle(const snk_position *position, const snk_field_obstacle *obstacle)
{
    return (position->x >= obstacle->top_left.x &&
            position->y >= obstacle->top_left.y &&
            position->x <= obstacle->bottom_right.x &&
            position->y <= obstacle->bottom_right.y);
}

static snk_rc_type
snk_check_position_available(const snk_position *position, const snk_field *field)
{
    snk_rc_type rc;
    uint32_t i;

    rc = snk_check_position_possible(position, field->width, field->height);
    if (rc != SNK_RC_SUCCESS)
        return rc;

    for (i = 0; i < field->n_obstacles; i++)
    {
        if (snk_position_within_obstacle(position, &field->obstacles[i]))
            return SNK_RC_INVALID;
    }

    return SNK_RC_SUCCESS;
}

static snk_rc_type
snk_check_field_obstacle_possible(const snk_field_obstacle *obstacle, uint32_t field_width, uint32_t field_height)
{
    snk_rc_type rc;

    rc = snk_check_position_possible(&obstacle->top_left, field_width, field_height);
    if (rc == SNK_RC_SUCCESS)
        rc = snk_check_position_possible(&obstacle->bottom_right, field_width, field_height);

    return rc;
}

snk_rc_type
snk_create_field(uint32_t width, uint32_t height, uint32_t n_obstacles, const snk_field_obstacle *obstacles,
                 uint32_t rand_seed, snk_field *field)
{
    size_t i;
    snk_rc_type rc;

    if (n_obstacles > SNK_ARRAY_LEN(field->obstacles))
        return SNK_RC_NOBUF;

    for (i = 0; i < n_obstacles; i++)
    {
        rc = snk_check_field_obstacle_possible(&obstacles[i], width, height);
        if (rc != SNK_RC_SUCCESS)
            return rc;
    }

    for (i = 0; i < n_obstacles; i++)
        field->obstacles[i] = obstacles[i];

    field->height = height;
    field->width = width;
    field->n_obstacles = n_obstacles;
    field->n_food = 0;
    field->rand_seed = rand_seed;

    return SNK_RC_SUCCESS;
}

struct snk_check_snake_data {
    const snk_field *field;
    snk_position positions[64];
    uint32_t n_positions;
};

static snk_rc_type
snk_check_snake_cb(const snk_position *pos, void *data)
{
    struct snk_check_snake_data *check_data = data;
    uint32_t i;
    snk_rc_type rc;

    rc = snk_check_position_available(pos, check_data->field);
    if (rc != SNK_RC_SUCCESS)
        return rc;

    for (i = 0; i < check_data->n_positions; i++)
    {
        if (snk_position_compare(pos, &check_data->positions[i]) == 0)
            return SNK_RC_INVALID;
    }

    check_data->positions[check_data->n_positions] = *pos;
    check_data->n_positions++;

    return SNK_RC_SUCCESS;
}

static snk_rc_type
snk_check_snake(const snk_snake *snake, const snk_field *field)
{
    struct snk_check_snake_data data = {field, {0}, 0};

    return snk_snake_walk(snake, snk_check_snake_cb, &data);
}

snk_rc_type
snk_create(const snk_field *field, const snk_position *start_position,
           snk_direction start_direction, uint32_t start_length, snk_process *process)
{
    snk_process result;
    snk_rc_type rc;

    if (start_length == 0)
        return SNK_RC_INVALID;

    result.field = *field;

    snk_snake_init(start_position, start_direction, NULL, start_length, 0, &result.snake);

    rc = snk_check_snake(&result.snake, &result.field);
    if (rc != SNK_RC_SUCCESS)
        return rc;

    result.next_direction = start_direction;
    result.state = SNK_STATE_RUNNING;

    *process = result;

    return SNK_RC_SUCCESS;
}

static snk_rc_type
snk_snake_advance_in_field(snk_snake *snake, snk_direction next_direction, snk_field *field)
{
    snk_snake snake_copy = *snake;
    snk_rc_type rc;

    rc = snk_snake_advance(&snake_copy, next_direction);
    if (rc != SNK_RC_SUCCESS)
        return rc;

    rc = snk_check_snake(&snake_copy, field);
    if (rc != SNK_RC_SUCCESS)
        return (rc == SNK_RC_INVALID ? SNK_RC_OVER : rc);

    if (field->n_food > 0 &&
        snk_position_compare(snk_snake_get_head_position(&snake_copy), &field->food) == 0)
    {
        snk_snake_add_pending_length(&snake_copy, 1);
        field->n_food = 0;
    }

    *snake = snake_copy;

    return SNK_RC_SUCCESS;
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
        return SNK_RC_SUCCESS;

    pos.x = snk_rand(&field->rand_seed) % field->width;
    pos.y = snk_rand(&field->rand_seed) % field->height;

    rc = snk_check_position_available(&pos, field);
    if (rc != SNK_RC_SUCCESS)
        return SNK_RC_SUCCESS;

    rc = snk_snake_get_positions(snake, &n_snk_positions, snk_positions);
    if (rc != SNK_RC_SUCCESS)
        return rc;

    for (i = 0; i < n_snk_positions; i++)
    {
        if (snk_position_compare(&pos, &snk_positions[i]) == 0)
            return SNK_RC_SUCCESS;
    }

    if (field->n_food > 0 && (snk_position_compare(&pos, &field->food) == 0))
        return SNK_RC_SUCCESS;

    field->n_food = 1;
    field->food = pos;

    return SNK_RC_SUCCESS;
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

    rc = snk_snake_advance_in_field(&process->snake, process->next_direction, &process->field);
    if (rc != SNK_RC_SUCCESS)
    {
        if (rc == SNK_RC_OVER)
            process->state = SNK_STATE_OVER;

        return rc;
    }

    rc = snk_generate_food(&process->snake, &process->field);
    if (rc != SNK_RC_SUCCESS)
        return rc;

    return SNK_RC_SUCCESS;
}

snk_rc_type
snk_choose_direction(snk_process *process, snk_direction direction)
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

    process->next_direction = direction;

    return SNK_RC_SUCCESS;
}

static snk_rc_type
snk_render_position(const snk_field *field, const snk_position *pos,
                    snk_position_type type, uint8_t *data, size_t data_size)
{
    size_t index = (pos->y * field->width) + pos->x;

    if (index >= data_size)
        return SNK_RC_NOBUF;

    data[index] = type;

    return SNK_RC_SUCCESS;
}

static snk_rc_type
snk_render_field_food(const snk_field *field, uint8_t *data, size_t data_size)
{
    snk_rc_type rc;

    if (field->n_food > 0)
    {
        rc = snk_render_position(field, &field->food, SNK_POSITION_FOOD, data, data_size);
        if (rc != SNK_RC_SUCCESS)
            return rc;
    }

    return SNK_RC_SUCCESS;
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
            if (rc != SNK_RC_SUCCESS)
                return rc;

            snk_position_advance(&pos, SNK_DIRECTION_RIGHT);
        }

        pos = row_start;
        snk_position_advance(&pos, SNK_DIRECTION_DOWN);
    }

    return SNK_RC_SUCCESS;
}
struct snk_render_data {
    const snk_field *field;
    uint8_t *data;
    size_t data_size;
};

static snk_rc_type
snk_render_cb(const snk_position *pos, void *data)
{
    struct snk_render_data *render_data = data;

    return snk_render_position(render_data->field, pos, SNK_POSITION_SNAKE, render_data->data, render_data->data_size);
}

snk_rc_type
snk_render(const snk_process *process, uint8_t *data, size_t data_size)
{
    struct snk_render_data render_data = {&process->field, data, data_size};
    size_t i;
    snk_rc_type rc;

    if ((process->field.height * process->field.width) > data_size)
        return SNK_RC_NOBUF;

    for (i = 0; i < data_size; i++)
        data[i] = SNK_POSITION_EMPTY;

    for (i = 0; i < process->field.n_obstacles; i++)
    {
        rc = snk_render_field_obstacle(&process->field, &process->field.obstacles[i], data, data_size);
        if (rc != SNK_RC_SUCCESS)
            return rc;
    }

    for (i = 0; i < process->field.n_food; i++)
    {
        rc = snk_render_field_food(&process->field, data, data_size);
        if (rc != SNK_RC_SUCCESS)
            return rc;
    }

    return snk_snake_walk(&process->snake, snk_render_cb, &render_data);
}

snk_rc_type snk_get_score(snk_process *process, snk_score *score)
{
    *score = snk_snake_get_length(&process->snake);

    return SNK_RC_SUCCESS;
}