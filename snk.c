#include "snk.h"
#include "snk_util.h"

int
snk_position_advance(snk_position *position, snk_direction direction)
{
    //printf("pos_advance: %d\n", direction);
    switch (direction)
    {
        case SNK_LEFT:
            position->x--;
            break;
        case SNK_RIGHT:
            position->x++;
            break;
        case SNK_UP:
            position->y--;
            break;
        case SNK_DOWN:
            position->y++;
            break;
        default:
            return EINVAL;
    }

    return 0;
}

int
snk_position_compare(const snk_position *a, const snk_position *b)
{
    return !(a->y == b->y && a->x == b->x);
}

static int
snk_check_position_possible(const snk_position *position, uint8_t field_width, uint8_t field_height)
{
    if (position->x < 0 || position->x >= field_width)
        return EINVAL;

    if (position->y < 0 || position->y >= field_height)
        return EINVAL;

    return 0;
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
snk_check_position_available(const snk_position *position, const snk_field *field)
{
    int rc;
    uint8_t i;

    rc = snk_check_position_possible(position, field->width, field->height);
    if (rc != 0)
        return rc;

    for (i = 0; i < field->n_obstacles; i++)
    {
        rc = snk_position_within_obstacle(position, &field->obstacles[i]);
        if (rc)
            return EINVAL;
    }

    return 0;
}

static int
snk_check_field_obstacle_possible(const snk_field_obstacle *obstacle, uint8_t field_width, uint8_t field_height)
{
    int rc;

    rc = snk_check_position_possible(&obstacle->top_left, field_width, field_height);
    if (rc == 0)
        rc = snk_check_position_possible(&obstacle->bottom_right, field_width, field_height);

    return rc;
}

int
snk_create_field(uint8_t width, uint8_t height, uint8_t n_obstacles, const snk_field_obstacle *obstacles, snk_field *field)
{
    size_t i;
    int rc;

    if (n_obstacles > SNK_ARRAY_LEN(field->obstacles))
        return EPERM;

    for (i = 0; i < n_obstacles; i++)
    {
        rc = snk_check_field_obstacle_possible(&obstacles[i], width, height);
        if (rc != 0)
            return rc;
    }

    for (i = 0; i < n_obstacles; i++)
        field->obstacles[i] = obstacles[i];

    field->height = height;
    field->width = width;
    field->n_obstacles = n_obstacles;
    field->n_food = 0;

    return 0;
}

struct snk_check_snake_data {
    const snk_field *field;
    snk_position positions[64];
    uint8_t n_positions;
};

static int
snk_check_snake_cb(const snk_position *pos, void *data)
{
    struct snk_check_snake_data *check_data = data;
    uint8_t i;
    int rc;

    rc = snk_check_position_available(pos, check_data->field);
    if (rc != 0)
        return rc;

    for (i = 0; i < check_data->n_positions; i++)
    {
        if (snk_position_compare(pos, &check_data->positions[i]) == 0)
            return EINVAL;
    }

    check_data->positions[check_data->n_positions] = *pos;
    check_data->n_positions++;

    return 0;
}

static int
snk_check_snake(const snk_snake *snake, const snk_field *field)
{
    struct snk_check_snake_data data = {field, {0}, 0};

    return snk_snake_walk(snake, snk_check_snake_cb, &data);
}

void
snk_snake_init(const snk_position *pos, snk_direction direction, const snk_joint_buffer *joints,
               uint16_t length, uint16_t pending_length, snk_snake *snake)
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

int
snk_create(const snk_field *field, const snk_position *start_position,
           snk_direction start_direction, uint16_t start_length, snk_process *process)
{
    snk_process result;
    int rc;

    if (start_length == 0)
        return EINVAL;

    result.field = *field;

    snk_snake_init(start_position, start_direction, NULL, start_length, 0, &result.snake);

    rc = snk_check_snake(&result.snake, &result.field);
    if (rc != 0)
        return rc;

    result.state = SNK_STATE_READY;
    result.next_direction = start_direction;

    *process = result;

    return 0;
}

int
snk_start(snk_process *process)
{
    switch (process->state)
    {
        case SNK_STATE_READY:
            process->state = SNK_STATE_RUNNING;
            return 0;
        case SNK_STATE_RUNNING:
        default:
            return EINVAL;
    }
}

static int
snk_snake_advance_in_field(snk_snake *snake, snk_direction next_direction, snk_field *field)
{
    snk_snake snake_copy = *snake;
    int rc;

    rc = snk_snake_advance(&snake_copy, next_direction);
    if (rc != 0)
        return rc;

    if (field->n_food > 0 && snk_position_compare(&snake_copy.head_position, &field->food) == 0)
    {
        snk_snake_add_pending_length(&snake_copy, 1);
        field->n_food = 0;
    }

    rc = snk_check_snake(&snake_copy, field);
    if (rc != 0)
        return rc;

    *snake = snake_copy;

    return 0;
}

static int
snk_generate_food(const snk_snake *snake, snk_field *field)
{
    snk_position pos = {3, 3};
    snk_position snk_positions[64];
    size_t n_snk_positions = SNK_ARRAY_LEN(snk_positions);
    size_t i;
    int rc;

    rc = snk_check_position_available(&pos, field);
    if (rc != 0)
        return 0;

    rc = snk_snake_get_positions(snake, &n_snk_positions, snk_positions);
    if (rc != 0)
        return rc;

    for (i = 0; i < n_snk_positions; i++)
    {
        if (snk_position_compare(&pos, &snk_positions[i]) == 0)
            return 0;
    }

    if (field->n_food > 0 && (snk_position_compare(&pos, &field->food) == 0))
        return 0;

    field->n_food = 1;
    field->food = pos;

    return 0;
}

int
snk_next_tick(snk_process *process)
{
    int rc;

    switch (process->state)
    {
        case SNK_STATE_RUNNING:
            break;
        case SNK_STATE_READY:
        default:
            return EINVAL;
    }

    rc = snk_snake_advance_in_field(&process->snake, process->next_direction, &process->field);
    if (rc != 0)
        return rc;

    rc = snk_generate_food(&process->snake, &process->field);
    if (rc != 0)
        return rc;

    return 0;
}

int
snk_choose_direction(snk_process *process, snk_direction direction)
{
    process->next_direction = direction;

    return 0;
}

static int
snk_render_position(const snk_field *field, const snk_position *pos, uint8_t *data, size_t data_size)
{
    size_t index = (pos->y * field->width) + pos->x;

    if (index >= data_size)
        return EPERM;

    data[index] = 'x';

    return 0;
}

static int
snk_render_field_obstacle(const snk_field *field, const snk_field_obstacle *obstacle, uint8_t *data, size_t data_size)
{
    snk_position pos = obstacle->top_left;
    snk_position row_start;
    int rc;

    while (obstacle->bottom_right.y >= pos.y)
    {
        row_start = pos;

        while (obstacle->bottom_right.x >= pos.x)
        {
            rc = snk_render_position(field, &pos, data, data_size);
            if (rc != 0)
                return rc;

            snk_position_advance(&pos, SNK_RIGHT);
        }

        pos = row_start;
        snk_position_advance(&pos, SNK_DOWN);
    }

    return 0;
}
struct snk_render_data {
    const snk_field *field;
    uint8_t *data;
    size_t data_size;
};

static int
snk_render_cb(const snk_position *pos, void *data)
{
    struct snk_render_data *render_data = data;

    return snk_render_position(render_data->field, pos, render_data->data, render_data->data_size);
}

int
snk_render(const snk_process *process, uint8_t *data, size_t data_size)
{
    struct snk_render_data render_data = {&process->field, data, data_size};
    size_t i;
    int rc;

    if ((size_t)(process->field.height * process->field.width) > data_size)
        return EPERM;

    for (i = 0; i < data_size; i++)
        data[i] = '0';

    for (i = 0; i < process->field.n_obstacles; i++)
    {
        rc = snk_render_field_obstacle(&process->field, &process->field.obstacles[i], data, data_size);
        if (rc != 0)
            return rc;
    }

    return snk_snake_walk(&process->snake, snk_render_cb, &render_data);
}