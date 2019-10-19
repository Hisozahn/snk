#include "snk_joint.h"
#include "snk_util.h"

void
snk_joint_init(const snk_position *position, snk_direction direction, snk_joint *joint)
{
    joint->position = *position;
    joint->direction = direction;
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
snk_joint_buffer_init(snk_joint_buffer *buffer) {
    buffer->first_joint = 0;
    buffer->n_joints = 0;
}

static uint32_t
get_index_in_buffer(const snk_joint_buffer *buffer, uint32_t i) {
    uint32_t buf_len = SNK_ARRAY_LEN(buffer->joints);

    return (buffer->first_joint + i) % buf_len;
}

snk_rc_type
snk_joint_buffer_get(const snk_joint_buffer *buffer, uint32_t i, snk_joint *joint) {
    uint32_t index;

    if (i >= buffer->n_joints)
        return SNK_RC_NOENT;

    index = get_index_in_buffer(buffer, i);
    *joint = buffer->joints[index];

    return SNK_RC_SUCCESS;
}

snk_rc_type
snk_joint_buffer_add(snk_joint_buffer *buffer, snk_joint *joint) {
    uint32_t index;

    if (buffer->n_joints + 1U > SNK_ARRAY_LEN(buffer->joints))
        return SNK_RC_NOBUF;

    index = get_index_in_buffer(buffer, (uint32_t)-1);
    buffer->joints[index] = *joint;
    buffer->n_joints++;
    buffer->first_joint = index;

    return SNK_RC_SUCCESS;
}

snk_rc_type
snk_joint_buffer_del(snk_joint_buffer *buffer) {
    if (buffer->n_joints == 0)
        return SNK_RC_INVALID;

    buffer->n_joints--;

    return SNK_RC_SUCCESS;
}

uint32_t
snk_joint_buffer_size(const snk_joint_buffer *buffer)
{
    return buffer->n_joints;
}