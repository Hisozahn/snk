#ifndef __SNK_JOINT_H__
#define __SNK_JOINT_H__

#include "snk_defs.h"
#include "snk_position.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Must fit into snk_joint_buffer's n_joints */
/** Maximum number of joints that can fit into a single joint buffer */
#define SNK_JOINTS_MAX 32

/**
 * Joint of a snake.
 *
 * When a snake turns it creates a joint in the position of the head before the turn.
 */
typedef struct snk_joint {
    snk_position position; /**< Position of a joint */
    snk_direction direction; /**< Direction of a joint towards the tail of a snake */
    uint32_t length; /**< Lenght of the straight part of the snake assigned to the joint */
} snk_joint;

/**
 * Collection of joints. A queue that supports operations:
 * 1) Add to head;
 * 2) Delete from tail;
 * 3) Get by index, 0 means head;
 */
typedef struct snk_joint_buffer {
    snk_joint joints[SNK_JOINTS_MAX]; /**< Joints array */
    uint32_t n_joints; /**< Current number of joints storing in the array */
    uint32_t first_joint; /**< Index of the first joint in the array */
} snk_joint_buffer;

/**
 * Initialize a joint.
 *
 * @note Initialization parameters are described in snk_joint
 *
 * @param[out] joint        Joint to initialize
 */
void snk_joint_init(const snk_position *position, snk_direction direction, uint32_t length, snk_joint *joint);

/** Joint's position getter */
const snk_position *snk_joint_get_position(const snk_joint *joint);

/** Joint's direction getter */
snk_direction snk_joint_get_direction(const snk_joint *joint);

/**
 * Initialize a collection of joints.
 *
 * @param[out] buffer    Joint buffer to initialize
 */
void snk_joint_buffer_init(snk_joint_buffer *buffer);

/**
 * Get an element from a joint buffer by index.
 *
 * @param[in] buffer    Buffer to get a joint from
 * @param[in] i         Index of an element to get, starting from the head
 *
 * @return              Pointer to a joint
 * @retval @c NULL      Index is out of bounds
 */
const snk_joint * snk_joint_buffer_get(const snk_joint_buffer *buffer, uint32_t i);

/** Wrapper for snk_joint_buffer_get() to get mutable joint */
snk_joint * snk_joint_buffer_get_mutable(snk_joint_buffer *buffer, uint32_t i);

/**
 * Check whether the next snk_joint_buffer_add() call to a @p buffer will succeed.
 *
 * @param[in] buffer    Joint buffer
 *
 * @return                  Status code
 * @retval SNK_RC_NOBUF     Not enough space in buffer to store an element
 */
snk_rc_type snk_joint_buffer_add_check(const snk_joint_buffer *buffer);

/**
 * Add an element to the head of a joint buffer.
 *
 * @note    Use snk_joint_buffer_add_check() to check whether this operation will succeed.
 *
 * @param[in,out] buffer    Buffer to add a joint to
 * @param[in] joint         Joint to add
 */
void snk_joint_buffer_add(snk_joint_buffer *buffer, snk_joint *joint);

/**
 * Get the number of elements currently inside a buffer.
 *
 * @param buffer[in]    Buffer
 *
 * @return              Number of elements
 */
uint32_t snk_joint_buffer_size(const snk_joint_buffer *buffer);

/**
 * Delete an element from the tail of a joint buffer.
 *
 * @param[in,out] buffer    Buffer to delete element from
 *
 * @return                  Status code
 * @retval SNK_RC_INVALID   Buffer is empty
 */
snk_rc_type snk_joint_buffer_del(snk_joint_buffer *buffer);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_JOINT_H__ */
