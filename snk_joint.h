#ifndef __SNK_JOINT_H__
#define __SNK_JOINT_H__

#include "snk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SNK_JOINTS_MAX 32

typedef struct snk_joint {
    snk_position position;
    snk_direction direction;
} snk_joint;

typedef struct snk_joint_buffer {
    snk_joint joints[SNK_JOINTS_MAX];
    uint8_t n_joints;
    uint8_t first_joint;
} snk_joint_buffer;

int snk_joint_buffer_init(snk_joint_buffer *buffer);
int snk_joint_get(const snk_joint_buffer *buffer, uint8_t i, snk_joint *joint);
int snk_joint_add(snk_joint_buffer *buffer, snk_joint *joint);
int snk_joint_del(snk_joint_buffer *buffer);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_JOINT_H__ */
