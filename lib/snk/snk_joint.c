#include "snk_joint.h"
#include "snk_util.h"

void
snk_joint_init(const snk_position *position, snk_direction direction, uint32_t length, snk_joint *joint)
{
    joint->position = *position;
    joint->direction = direction;
    joint->length = length;
}

const snk_position *
snk_joint_get_position(const snk_joint *joint)
{
    return &joint->position;
}

snk_direction snk_joint_get_direction(const snk_joint *joint)
{
    return joint->direction;
}

void
snk_joint_buffer_init(uint8_t wrap_joints, snk_joint_buffer *buffer) {
    buffer->wrap_joints = wrap_joints;
    buffer->first_joint = 0;
    buffer->n_joints = 0;
}

static uint32_t
get_index_in_buffer(const snk_joint_buffer *buffer, uint32_t i) {
    uint32_t buf_len = SNK_ARRAY_LEN(buffer->joints);

    return (buffer->first_joint + i) % buf_len;
}

#define SNK_JOINT_BUFFER_GET(_buffer, i)            \
    do {                                            \
        uint32_t index;                             \
        if (i >= _buffer->n_joints)                 \
            return NULL;                            \
                                                    \
        index = get_index_in_buffer(_buffer, i);    \
        return &_buffer->joints[index];             \
    } while (0)

const snk_joint *
snk_joint_buffer_get(const snk_joint_buffer *buffer, uint32_t i)
{
    SNK_JOINT_BUFFER_GET(buffer, i);
}

snk_joint *
snk_joint_buffer_get_mutable(snk_joint_buffer *buffer, uint32_t i)
{
    SNK_JOINT_BUFFER_GET(buffer, i);
}

snk_rc_type
snk_joint_buffer_add_check(const snk_joint_buffer *buffer)
{
    if (!buffer->wrap_joints && (buffer->n_joints + 1U > SNK_ARRAY_LEN(buffer->joints)))
        return SNK_RC_NOBUF;

    return 0;
}

void
snk_joint_buffer_add(snk_joint_buffer *buffer, snk_joint *joint) {
    uint32_t index;

    index = get_index_in_buffer(buffer, (uint32_t)-1);
    buffer->joints[index] = *joint;

    if (buffer->n_joints < SNK_ARRAY_LEN(buffer->joints))
        buffer->n_joints++;

    buffer->first_joint = index;
}

snk_rc_type
snk_joint_buffer_del(snk_joint_buffer *buffer) {
    if (buffer->n_joints == 0)
        return SNK_RC_INVALID;

    buffer->n_joints--;

    return 0;
}

uint32_t
snk_joint_buffer_size(const snk_joint_buffer *buffer)
{
    return buffer->n_joints;
}