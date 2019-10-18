#include <errno.h>
#include "snk_joint.h"
#include "snk_util.h"

void
snk_joint_buffer_init(snk_joint_buffer *buffer) {
    buffer->first_joint = 0;
    buffer->n_joints = 0;
}

static uint8_t
get_index_in_buffer(const snk_joint_buffer *buffer, uint8_t i) {
    uint8_t buf_len = SNK_ARRAY_LEN(buffer->joints);

    return (buffer->first_joint + i) % buf_len;
}

int
snk_joint_get(const snk_joint_buffer *buffer, uint8_t i, snk_joint *joint) {
    uint8_t index;

    //printf("joint_get: %u\n", i);
    if (i >= buffer->n_joints)
        return ENOENT;

    index = get_index_in_buffer(buffer, i);
    *joint = buffer->joints[index];

    return 0;
}

int
snk_joint_add(snk_joint_buffer *buffer, snk_joint *joint) {
    uint8_t index;

    if (buffer->n_joints + 1U > SNK_ARRAY_LEN(buffer->joints))
        return EPERM;

    index = get_index_in_buffer(buffer, (uint8_t)-1);
    //printf("index: %u\n", index);
    buffer->joints[index] = *joint;
    buffer->n_joints++;
    buffer->first_joint = index;

    return 0;
}

int
snk_joint_del(snk_joint_buffer *buffer) {
    if (buffer->n_joints == 0)
        return EINVAL;

    buffer->n_joints--;

    return 0;
}

uint8_t
snk_joint_size(const snk_joint_buffer *buffer)
{
    return buffer->n_joints;
}