#include "snk_joint.h"
#include "snk_util.h"

int
snk_joint_buffer_init(snk_joint_buffer *buffer)
{
    (void)buffer;
    /* TODO: check if memset is working */
    return 0;
}

static uint8_t
get_valid_index(const snk_joint_buffer *buffer, int i)
{
    int index;
    size_t buf_len;

    buf_len = SNK_ARRAY_LEN(buffer->joints);
    index = i + buf_len;

    snk_assert(i >= 0);

    return index % buf_len;
}

int
snk_joint_get(const snk_joint_buffer *buffer, uint8_t i, snk_joint *joint)
{
    uint8_t result_index;

    if (i >= buffer->n_joints)
        return EINVAL;

    result_index = get_valid_index(buffer, buffer->first_joint + i);
    *joint = buffer->joints[result_index];

    return 0;
}

int
snk_joint_add(snk_joint_buffer *buffer, snk_joint *joint)
{
    uint8_t result_index;

    if (buffer->n_joints + 1U > SNK_ARRAY_LEN(buffer->joints))
        return ENOBUFS;

    result_index = get_valid_index(buffer, (int)buffer->first_joint - 1);

    buffer->first_joint = result_index;
    buffer->joints[result_index] = *joint;
    buffer->n_joints++;

    return 0;
}

int
snk_joint_del(snk_joint_buffer *buffer)
{
    if (buffer->n_joints == 0)
        return EINVAL;

    buffer->n_joints--;

    return 0;
}
