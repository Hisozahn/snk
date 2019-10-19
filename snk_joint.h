#ifndef __SNK_JOINT_H__
#define __SNK_JOINT_H__

#include "snk_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Must fit into snk_joint_buffer's n_joints */
#define SNK_JOINTS_MAX 32

typedef struct snk_joint {
    snk_position position;
    snk_direction direction;
} snk_joint;

typedef struct snk_joint_buffer {
    snk_joint joints[SNK_JOINTS_MAX];
    uint32_t n_joints;
    uint32_t first_joint;
} snk_joint_buffer;

void snk_joint_init(const snk_position *position, snk_direction direction, snk_joint *joint);
const snk_position *snk_joint_get_position(const snk_joint *joint);
snk_direction snk_joint_get_direction(const snk_joint *joint);

void snk_joint_buffer_init(snk_joint_buffer *buffer);
snk_rc_type snk_joint_buffer_get(const snk_joint_buffer *buffer, uint32_t i, snk_joint *joint);
snk_rc_type snk_joint_buffer_add(snk_joint_buffer *buffer, snk_joint *joint);
uint32_t snk_joint_buffer_size(const snk_joint_buffer *buffer);
snk_rc_type snk_joint_buffer_del(snk_joint_buffer *buffer);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_JOINT_H__ */
