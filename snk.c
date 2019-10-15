#include "snk.h"
#include "snk_joint.h"
#include "snk_util.h"

#define SNK_FIELD_DATA_MAX 2048

typedef enum snk_state {
    SNK_STATE_READY,
    SNK_STATE_RUNNING,
} snk_state;

/** Snake description */
typedef struct snk_snake {
    snk_position head_position;
    snk_direction head_direction;
    snk_joint_buffer joints;
    uint16_t length;
    uint16_t pending_length;
} snk_snake;

struct snk_process {
    snk_field field;
    uint8_t field_data[SNK_FIELD_DATA_MAX];
    snk_snake snake;
    snk_state state;
};

static int
snk_check_position(const snk_position *position, uint8_t field_width, uint8_t field_height)
{
    if (position->x < 0 || position->x >= field_width)
        return EINVAL;

    if (position->y < 0 || position->y >= field_height)
        return EINVAL;

    return 0;
}

static int
snk_check_field_obstacle(const snk_field_obstacle *obstacle, uint8_t field_width, uint8_t field_height)
{
    int rc;

    rc = snk_check_position(&obstacle->top_left, field_width, field_height);
    if (rc == 0)
        rc = snk_check_position(&obstacle->bottom_right, field_width, field_height);

    return rc;
}

int
snk_create_field(uint8_t width, uint8_t height, uint8_t n_obstacles, const snk_field_obstacle *obstacles, snk_field *field)
{
    size_t i;
    int rc;

    if (n_obstacles > SNK_ARRAY_LEN(field->obstacles))
        return ENOBUFS;

    for (i = 0; i < n_obstacles; i++)
    {
        rc = snk_check_field_obstacle(&obstacles[i], width, height);
        if (rc != 0)
            return rc;
    }

    for (i = 0; i < n_obstacles; i++)
        field->obstacles[i] = obstacles[i];

    field->height = height;
    field->width = width;
    field->n_obstacles = n_obstacles;

    return 0;
}


static int
snk_check_snake(const snk_snake *snake, const snk_field *field)
{
    uint16_t i;
    uint8_t joint_i;
    snk_joint joint;
    int rc;

    /* TODO: implement */
    rc = snk_check_position(&snake->head_position, field->width, field->height);
    if (rc != 0)
        return rc;

    for (i = 1, joint_i = 0; i < snake->length; i++)
    {
        snk_joint_get(&snake->joints, joint_i, &joint);
    }

    return ENOTSUP;
}

void
snk_snake_init(const snk_position *pos, const snk_direction *direction, const snk_joint_buffer *joints,
               uint16_t length, uint16_t pending_length, snk_snake *snake)
{
    if (joints != NULL)
        snake->joints = *joints;
    else
        snk_joint_buffer_init(&snake->joints);

    snake->head_direction = *direction;
    snake->head_position = *pos;
    snake->length = length;
    snake->pending_length = pending_length;
}

int
snk_create(const snk_field *field, const snk_position *start_position,
           const snk_direction *start_direction, uint16_t start_length, snk_process *process)
{
    snk_process result;
    int rc;

    if (start_length == 0)
        return EINVAL;

    if ((field->width * field->height) > SNK_ARRAY_LEN(result.field_data))
        return EINVAL;

  
    result.field = *field;

    snk_snake_init(start_position, start_direction, NULL, 0, 0, &result.snake);

    rc = snk_check_snake(&result.snake, &result.field);
    if (rc != 0)
        return rc;

    result.state = SNK_STATE_READY;

    *process = result;

    return 0;
}

int
snk_start(snk_process *process)
{
    (void)process;

    return ENOTSUP;
}

int
snk_next_tick(snk_process *process)
{
    (void)process;

    return ENOTSUP;
}

int
snk_choose_direction(snk_process *process, snk_direction direction)
{
    (void)process;
    (void)direction;

    return ENOTSUP;
}
