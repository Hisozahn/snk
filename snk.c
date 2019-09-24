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
snk_check_position(const snk_position *position, const snk_field *field)
{
    if (position->x < 0 || (size_t)position->x >= field->width)
        return EINVAL;

    if (position->y < 0 || (size_t)position->y >= field->height)
        return EINVAL;

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
    rc = snk_check_position(&snake->head_position, field);
    if (rc != 0)
        return rc;

    for (i = 1, joint_i = 0; i < snake->length; i++)
    {
        snk_joint_get(&snake->joints, joint_i, &joint);
    }

    return ENOTSUP;
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

    result.snake.head_position = *start_position;
    result.snake.head_direction = *start_direction;
    result.snake.length = start_length;
    rc = snk_joint_buffer_init(&result.snake.joints);
    if (rc != 0)
        return rc;

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
